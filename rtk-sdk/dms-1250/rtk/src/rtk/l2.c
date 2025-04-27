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
 * Purpose : Definition of L2 API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Mac address flush
 *           (2) Address learning limit
 *           (3) Parameter for L2 lookup and learning engine
 *           (4) Unicast address
 *           (5) L2 multicast
 *           (6) IP multicast
 *           (7) Multicast forwarding table
 *           (8) CPU mac
 *           (9) Port move
 *           (10) Parameter for lookup miss
 *           (11) Parameter for MISC
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/l2.h>

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Module Name    : L2     */
/* Sub-module Name: Global */

/* Function Name:
 *      rtk_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize l2 module before calling any l2 APIs.
 * Changes:
 *      None
 */
int32
rtk_l2_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_init(unit);
} /* end of rtk_l2_init */

/* Module Name    : L2                */
/* Sub-module Name: Mac address flush */

/* Function Name:
 *      rtk_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush dynamic mac entries corresponding to linkdown port configuration.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer buffer of state of HW clear linkdown port mac
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 * Changes:
 *      None
 */
int32
rtk_l2_flushLinkDownPortAddrEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_flushLinkDownPortAddrEnable_get(unit, pEnable);
} /* end of rtk_l2_flushLinkDownPortAddrEnable_get */

/* Function Name:
 *      rtk_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush dynamic mac entries corresponding to linkdown port configuration.
 * Input:
 *      unit   - unit id
 *      enable - configure value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 * Changes:
 *      None
 */
int32
rtk_l2_flushLinkDownPortAddrEnable_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_flushLinkDownPortAddrEnable_set(unit, enable);
} /* end of rtk_l2_flushLinkDownPortAddrEnable_set */

/* Function Name:
 *      rtk_l2_ucastAddr_flush
 * Description:
 *      Flush unicast address
 * Input:
 *      unit    - unit id
 *      pConfig - flush config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_DEV_ID       - invalid device id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The L2 unicast entries in L2 table which meet the specified criteria are flushed by HW.
 * Changes:
 *      None
 */
int32
rtk_l2_ucastAddr_flush(uint32 unit, rtk_l2_flushCfg_t *pConfig)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ucastAddr_flush(unit, pConfig);

} /* end of rtk_l2_ucastAddr_flush */

/* Module Name    : L2                     */
/* Sub-module Name: Address learning limit */

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      rtk_l2_learningCnt_get
 * Description:
 *      Get the total mac learning counts of specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pMac_cnt - pointer buffer of system mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_macLearningCnt_get(unit, type, pLimitCnt)
 *          Parameters:
 *              type                     -  L2_MAC_LIMIT_GLOBAL
 *              pMac_cnt->glbCnt         -  *pMac_cnt
 */
int32
rtk_l2_learningCnt_get(uint32 unit, uint32 *pMac_cnt)
{
    int32 ret;
    rtk_l2_macCnt_t limitCnt;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(pMac_cnt==NULL, RT_ERR_NULL_POINTER);

    if((ret = RT_MAPPER(unit)->l2_macLearningCnt_get(unit, L2_MAC_LIMIT_GLOBAL, &limitCnt)) != RT_ERR_OK)
        return ret;

    *pMac_cnt = limitCnt.glbCnt;
    return ret;
} /* end of rtk_l2_learningCnt_get */

/* Function Name:
 *      rtk_l2_portLearningCnt_get
 * Description:
 *      Get current mac learning counts of the port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pMac_cnt - pointer buffer of mac learning counts of the port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The mac learning counts returned only calculate dynamic mac numbers.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_macLearningCnt_get(unit, type, pLimitCnt)
 *          Parameters:
 *              type                     - L2_MAC_LIMIT_PORT
 *              pMac_cnt->portTrkCnt.id  - port
 *              pMac_cnt->portTrkCnt.cnt - *pMac_cnt
 */
int32
rtk_l2_portLearningCnt_get(uint32 unit, rtk_port_t port, uint32 *pMac_cnt)
{
    int32 ret;
    rtk_l2_macCnt_t limitCnt;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(pMac_cnt==NULL, RT_ERR_NULL_POINTER);

    limitCnt.portTrkCnt.id = port;

    if((ret = RT_MAPPER(unit)->l2_macLearningCnt_get(unit, L2_MAC_LIMIT_PORT, &limitCnt)) != RT_ERR_OK)
        return ret;

    *pMac_cnt = limitCnt.portTrkCnt.cnt;
    return ret;
}/* end of rtk_l2_portLearningCnt_get */

/* Function Name:
 *      rtk_l2_fidLearningCnt_get
 * Description:
 *      Get number of learned MAC addresses on specified VLAN MAC limit entry.
 * Input:
 *      unit             - unit id
 *      fid_macLimit_idx - index of VLAN MAC limit entry
 * Output:
 *      pNum             - number of learned MAC addresses
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_macLearningCnt_get(unit, type, pLimitCnt)
 *          Parameters:
 *              type                     -  L2_MAC_LIMIT_FID
 *              pMac_cnt->fidCnt.entryId -  fid_macLimit_idx
 *              pMac_cnt->fidCnt.cnt     -  *pMac_cnt
 */
int32
rtk_l2_fidLearningCnt_get(uint32 unit, uint32 fid_macLimit_idx, uint32 *pNum)
{
    int32 ret;
    rtk_l2_macCnt_t limitCnt;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(pNum==NULL, RT_ERR_NULL_POINTER);

    limitCnt.fidCnt.entryId = fid_macLimit_idx;

    if((ret = RT_MAPPER(unit)->l2_macLearningCnt_get(unit, L2_MAC_LIMIT_FID, &limitCnt)) != RT_ERR_OK)
        return ret;

    *pNum = limitCnt.fidCnt.cnt;
    return ret;
} /* end of rtk_l2_fidLearningCnt_get */
#endif

/* Function Name:
 *      rtk_l2_macLearningCnt_get
 * Description:
 *      Get number of learned MAC addresses of specified type.
 * Input:
 *      unit             - unit id
 *      fid_macLimit_idx - index of VLAN MAC limit entry
 * Output:
 *      pNum             - number of learned MAC addresses
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_l2_fidLearningCnt_get
 *              rtk_l2_learningCnt_get
 *              rtk_l2_portLearningCnt_get
 */
int32
rtk_l2_macLearningCnt_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_macLearningCnt_get(unit, type, pLimitCnt);

} /* end of rtk_l2_macLearningCnt_get */

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      rtk_l2_limitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pMac_cnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the mac_cnt to 0 mean disable learning in the system.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_limitLearningNum_get(unit, type, pLimitCnt)
 *          Parameters:
 *              type                     -  L2_MAC_LIMIT_GLOBAL
 *              pLimitCnt->glbCnt        -  *pMac_cnt
 */
int32
rtk_l2_limitLearningCnt_get(uint32 unit, uint32 *pMac_cnt)
{
    int32 ret;
    rtk_l2_macCnt_t limitCnt;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(pMac_cnt==NULL, RT_ERR_NULL_POINTER);

    if((ret = RT_MAPPER(unit)->l2_limitLearningNum_get(unit, L2_MAC_LIMIT_GLOBAL, &limitCnt)) != RT_ERR_OK)
        return ret;

    *pMac_cnt = limitCnt.glbCnt;
    return ret;
}  /* end of rtk_l2_limitLearningCnt_get */

/* Function Name:
 *      rtk_l2_limitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the specified device.
 * Input:
 *      unit    - unit id
 *      mac_cnt - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the mac_cnt to 0 mean disable learning in the system.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_limitLearningNum_set(unit, type, pLimitCnt)
 *          Parameters:
 *              type                     -  L2_MAC_LIMIT_GLOBAL
 *              pLimitCnt->glbCnt        -  mac_cnt
 */
int32
rtk_l2_limitLearningCnt_set(uint32 unit, uint32 mac_cnt)
{
    rtk_l2_macCnt_t limitCnt;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    limitCnt.glbCnt = mac_cnt;
    return RT_MAPPER(unit)->l2_limitLearningNum_set(unit, L2_MAC_LIMIT_GLOBAL, &limitCnt);
}  /* end of rtk_l2_limitLearningCnt_set */

/* Function Name:
 *      rtk_l2_portLimitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pMac_cnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the mac_cnt to 0 mean disable learning in the port.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_limitLearningNum_get(unit, type, pLimitCnt)
 *          Parameters:
 *              type                        - L2_MAC_LIMIT_PORT
 *              pLimitCnt->portTrkCnt.id    - port
 *              pLimitCnt->portTrkCnt.cnt   - *pMac_cnt
 */
int32
rtk_l2_portLimitLearningCnt_get(uint32 unit, rtk_port_t port, uint32 *pMac_cnt)
{
    int32 ret;
    rtk_l2_macCnt_t limitCnt;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(pMac_cnt==NULL, RT_ERR_NULL_POINTER);

    limitCnt.portTrkCnt.id = port;

    if((ret = RT_MAPPER(unit)->l2_limitLearningNum_get(unit, L2_MAC_LIMIT_PORT, &limitCnt)) != RT_ERR_OK)
        return ret;

    *pMac_cnt = limitCnt.portTrkCnt.cnt;
    return ret;
} /* end of rtk_l2_portLimitLearningCnt_get */

/* Function Name:
 *      rtk_l2_portLimitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mac_cnt - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the mac_cnt to 0 mean disable learning in the port.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_limitLearningNum_set(unit, type, pLimitCnt)
 *          Parameters:
 *              type                        - L2_MAC_LIMIT_PORT
 *              pLimitCnt->portTrkCnt.id    - port
 *              pLimitCnt->portTrkCnt.cnt   - mac_cnt
 */
int32
rtk_l2_portLimitLearningCnt_set(uint32 unit, rtk_port_t port, uint32 mac_cnt)
{
    rtk_l2_macCnt_t limitCnt;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    limitCnt.portTrkCnt.id = port;


    limitCnt.portTrkCnt.cnt = mac_cnt;
    return RT_MAPPER(unit)->l2_limitLearningNum_set(unit, L2_MAC_LIMIT_PORT, &limitCnt);
} /* end of rtk_l2_portLimitLearningCnt_set */
#endif

/* Function Name:
 *      rtk_l2_limitLearningNum_get
 * Description:
 *      Get the mac limit learning counts of specified device.
 * Input:
 *      unit        - unit id
 *      type        - mac limit type
 *      pLimitCnt   - pointer ro mac limit parameters
 * Output:
 *      pLimitCnt   - pointer of mac limit learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_l2_limitLearningCnt_get
 *              rtk_l2_portLimitLearningCnt_get
 */
int32
rtk_l2_limitLearningNum_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_limitLearningNum_get(unit, type, pLimitCnt);

} /* end of rtk_l2_limitLearningNum_get */

/* Function Name:
 *      rtk_l2_limitLearningNum_set
 * Description:
 *      Set the mac limit learning counts of specified device.
 * Input:
 * Input:
 *      unit        - unit id
 *      type        - mac limit type
 *      pLimitCnt   - pointer ro mac limit parameters and mac limit learning counts
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 *      (2) Set rtk_l2_macCnt_t.cnt=L2_MAC_CST_DISABLE to disable mac limit learning functionality.
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_l2_limitLearningCnt_set
 *              rtk_l2_portLimitLearningCnt_set
 */
int32
rtk_l2_limitLearningNum_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_limitLearningNum_set(unit, type, pLimitCnt);

} /* end of rtk_l2_limitLearningNum_set */

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      rtk_l2_limitLearningCntAction_get
 * Description:
 *      Get the action when over system learning maximum mac counts.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_limitLearningAction_get(unit, type, pAction)
 *          Parameters:
 *              type                     -  L2_MAC_LIMIT_GLOBAL
 *              pAction->glbAct          -  *pAction
 */
int32
rtk_l2_limitLearningCntAction_get(uint32 unit, rtk_l2_limitLearnCntAction_t *pAction)
{
    int32 ret;
    rtk_l2_macLimitAction_t limitAction;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(pAction==NULL, RT_ERR_NULL_POINTER);

    if((ret = RT_MAPPER(unit)->l2_limitLearningAction_get(unit, L2_MAC_LIMIT_GLOBAL, &limitAction)) != RT_ERR_OK)
        return ret;

    *pAction = limitAction.glbAct;
    return ret;
} /* end of rtk_l2_limitLearningCntAction_get */

/* Function Name:
 *      rtk_l2_limitLearningCntAction_set
 * Description:
 *      Set the action when over system learning maximum mac counts.
 * Input:
 *      unit   - unit id
 *      action - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_limitLearningAction_set(unit, type, action)
 *          Parameters:
 *              type                     -  L2_MAC_LIMIT_GLOBAL
 *              pAction->glbAct          -  action
 */
int32
rtk_l2_limitLearningCntAction_set(uint32 unit, rtk_l2_limitLearnCntAction_t action)
{
    rtk_l2_macLimitAction_t limitAction;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    limitAction.glbAct = action;
    return RT_MAPPER(unit)->l2_limitLearningAction_set(unit, L2_MAC_LIMIT_GLOBAL, &limitAction);
} /* end of rtk_l2_limitLearningCntAction_set */

/* Function Name:
 *      rtk_l2_portLimitLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of the port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_limitLearningAction_get(unit, type, pAction)
 *          Parameters:
 *              type                     -  L2_MAC_LIMIT_PORT
 *              pAction->portTrkAct.id   -  port
 *              pAction->portTrkAct.act  -  *pAction
 */
int32
rtk_l2_portLimitLearningCntAction_get(uint32 unit, rtk_port_t port, rtk_l2_limitLearnCntAction_t *pAction)
{
    int32 ret;
    rtk_l2_macLimitAction_t limitAction;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(pAction==NULL, RT_ERR_NULL_POINTER);

    limitAction.portTrkAct.id = port;


    if((ret = RT_MAPPER(unit)->l2_limitLearningAction_get(unit, L2_MAC_LIMIT_PORT, &limitAction)) != RT_ERR_OK)
        return ret;

    *pAction = limitAction.portTrkAct.act;
    return ret;
} /* end of rtk_l2_portLimitLearningCntAction_get */

/* Function Name:
 *      rtk_l2_portLimitLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of the port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_limitLearningAction_set(unit, type, action)
 *          Parameters:
 *              type                     -  L2_MAC_LIMIT_PORT
 *              pAction->portTrkAct.id   -  port
 *              pAction->portTrkAct.act  -  action
 */
int32
rtk_l2_portLimitLearningCntAction_set(uint32 unit, rtk_port_t port, rtk_l2_limitLearnCntAction_t action)
{
    rtk_l2_macLimitAction_t limitAction;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    limitAction.portTrkAct.id = port;


    limitAction.portTrkAct.act = action;
    return RT_MAPPER(unit)->l2_limitLearningAction_set(unit, L2_MAC_LIMIT_PORT, &limitAction);
} /* end of rtk_l2_portLimitLearningCntAction_set */

/* Function Name:
 *      rtk_l2_fidLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of VLAN MAC limit.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The action applied to all the VLAN MAC limit configuration.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_limitLearningAction_get(unit, type, pAction)
 *          Parameters:
 *              type                 -  L2_MAC_LIMIT_FID
 *              pAction->fidAct.act  -  *pAction
 */
int32
rtk_l2_fidLearningCntAction_get(uint32 unit, rtk_l2_limitLearnCntAction_t *pAction)
{
    int32 ret;
    rtk_l2_macLimitAction_t limitAction;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(pAction==NULL, RT_ERR_NULL_POINTER);

    if((ret = RT_MAPPER(unit)->l2_limitLearningAction_get(unit, L2_MAC_LIMIT_FID, &limitAction)) != RT_ERR_OK)
        return ret;

    *pAction = limitAction.fidAct.act;
    return ret;
} /* end of rtk_l2_fidLearningCntAction_get */

/* Function Name:
 *      rtk_l2_fidLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of VLAN MAC limit.
 * Input:
 *      unit   - unit id
 *      action - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The action applied to all the VLAN MAC limit configuration.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_limitLearningAction_set(unit, type, action)
 *          Parameters:
 *              type                 -  L2_MAC_LIMIT_FID
 *              pAction->fidAct.act  -  action
 */
int32
rtk_l2_fidLearningCntAction_set(uint32 unit, rtk_l2_limitLearnCntAction_t action)
{
    rtk_l2_macLimitAction_t limitAction;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    limitAction.fidAct.act = action;
    return RT_MAPPER(unit)->l2_limitLearningAction_set(unit, L2_MAC_LIMIT_FID, &limitAction);
} /* end of rtk_l2_fidLearningCntAction_set */
#endif

/* Function Name:
 *      rtk_l2_limitLearningAction_get
 * Description:
 *      Get the mac limit action of specified device.
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pAction - pointer ro mac limit parameters
 * Output:
 *      pAction - pointer of mac limit action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_l2_fidLearningCntAction_get
 *              rtk_l2_limitLearningCntAction_get
 *              rtk_l2_portLimitLearningCntAction_get
 */
int32
rtk_l2_limitLearningAction_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_limitLearningAction_get(unit, type, pAction);

} /* end of rtk_l2_limitLearningAction_get */

/* Function Name:
 *      rtk_l2_limitLearningAction_set
 * Description:
 *      Set the mac limit action of specified device.
 * Input:
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pAction - pointer ro mac limit parameters and mac limit action
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_l2_fidLearningCntAction_set
 *              rtk_l2_limitLearningCntAction_set
 *              rtk_l2_portLimitLearningCntAction_set
 */
int32
rtk_l2_limitLearningAction_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_limitLearningAction_set(unit, type, pAction);

} /* end of rtk_l2_limitLearningAction_set */

/* Function Name:
 *      rtk_l2_fidLimitLearningEntry_get
 * Description:
 *      Get the specified VLAN MAC limit configuration.
 * Input:
 *      unit              - unit id
 *      fid_macLimit_idx  - index of VLAN MAC limit entry
 * Output:
 *      pFidMacLimitEntry - pointer to MAC limit configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 *      (2) Forwarding action is as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 * Changes:
 *      [SDK_3.0.0]
 *          Support trunk ID for 9300/9310.
 */
int32
rtk_l2_fidLimitLearningEntry_get(
    uint32                    unit,
    uint32                    fid_macLimit_idx,
    rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_fidLimitLearningEntry_get(unit, fid_macLimit_idx, pFidMacLimitEntry);

} /* end of rtk_l2_fidLimitLearningEntry_get */

/* Function Name:
 *      rtk_l2_fidLimitLearningEntry_set
 * Description:
 *      Set the specified VLAN MAC limit configuration.
 * Input:
 *      unit              - unit id
 *      fid_macLimit_idx  - index of VLAN MAC limit entry
 *      pFidMacLimitEntry - MAC limit configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_VLAN_VID     - invalid vlan id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 *      (2) Forwarding action is as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 *      (3) This API will flush all MACs learnt on specified VLAN before this entry takes effect.
 *          If the fid of an entry changes from one to another, this API will flush MACs learnt on these two VLANs.
 * Changes:
 *      [SDK_3.0.0]
 *          Support trunk ID for 9300/9310.
 */
int32
rtk_l2_fidLimitLearningEntry_set(
    uint32                    unit,
    uint32                    fid_macLimit_idx,
    rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_fidLimitLearningEntry_set(unit, fid_macLimit_idx, pFidMacLimitEntry);

} /* end of rtk_l2_fidLimitLearningEntry_set */

/* Function Name:
 *      rtk_l2_fidLearningCnt_reset
 * Description:
 *      Reset number of learned MAC addresses on specified VLAN MAC limit entry.
 * Input:
 *      unit             - unit id
 *      fid_macLimit_idx - index of VLAN MAC limit entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 * Changes:
 *      None
 */
int32
rtk_l2_fidLearningCnt_reset(uint32 unit, uint32 fid_macLimit_idx)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_fidLearningCnt_reset(unit, fid_macLimit_idx);
} /* end of rtk_l2_fidLearningCnt_reset */

/* Module Name    : L2                                          */
/* Sub-module Name: Parameter for L2 lookup and learning engine */
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      rtk_l2_aging_get
 * Description:
 *      Get the dynamic address aging time from the specified device.
 * Input:
 *      unit        - unit id
 * Output:
 *      pAging_time - pointer buffer of aging time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The unit is second.
 *      (2) Get aging_time as 0 mean disable aging mechanism.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_agingTime_get(unit, type, pAging_time)
 *          Parameters:
 *              type                 -  L2_AGE_TIME_NORMAL
 *              pAging_time          -  pAging_time
 */
int32
rtk_l2_aging_get(uint32 unit, uint32 *pAging_time)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_agingTime_get(unit, L2_AGE_TIME_NORMAL, pAging_time);
} /* end of rtk_l2_aging_get */

/* Function Name:
 *      rtk_l2_aging_set
 * Description:
 *      Set the dynamic address aging time to the specified device.
 * Input:
 *      unit       - unit id
 *      aging_time - aging time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The unit is second.
 *      (2) apply aging_time as 0 mean disable aging mechanism.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_agingTime_set(unit, type, aging_time)
 *          Parameters:
 *              type                 -  L2_AGE_TIME_NORMAL
 *              aging_time           -  aging_time
 */

int32
rtk_l2_aging_set(uint32 unit, uint32 aging_time)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_agingTime_set(unit, L2_AGE_TIME_NORMAL, aging_time);
} /* end of rtk_l2_aging_set */
#endif

/* Function Name:
 *      rtk_l2_agingTime_get
 * Description:
 *      Get the aging time of from the specified device.
 * Input:
 *      unit        - unit id
 *      type       - age time type
 * Output:
 *      pAging_time - pointer buffer of aging time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      1. The unit is second.
 *      2. When specified type is L2_AGE_TIME_SUSPEND, the pAging_time may be some deviation from aging_time passed in rtk_l2_agingTime_set().
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_l2_aging_get
 */
int32
rtk_l2_agingTime_get(uint32 unit, rtk_l2_ageTimeType_t type, uint32 *pAging_time)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_agingTime_get(unit, type, pAging_time);
} /* end of rtk_l2_agingTime_get */

/* Function Name:
 *      rtk_l2_agingTime_set
 * Description:
 *      Set the aging time of suspending entry from the specified device.
 * Input:
 *      unit       - unit id
 *      type       - age time type
 *      aging_time - aging time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      1. The unit is second.
 *      2. When specified type is L2_AGE_TIME_SUSPEND, there may be some deviation between aging_time and the value write into reg.
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_l2_aging_set
 */
int32
rtk_l2_agingTime_set(uint32 unit, rtk_l2_ageTimeType_t type, uint32 aging_time)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_agingTime_set(unit, type, aging_time);
} /* end of rtk_l2_agingTime_set */

/* Function Name:
 *      rtk_l2_portAgingEnable_get
 * Description:
 *      Get the dynamic address aging out state configuration of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - enable status of aging out
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l2_portAgingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portAgingEnable_get(unit, port, pEnable);

} /* end of rtk_l2_portAgingEnable_get */

/* Function Name:
 *      rtk_l2_portAgingEnable_set
 * Description:
 *      Set the dynamic address aging out state configuration of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of aging out
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l2_portAgingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portAgingEnable_set(unit, port, enable);

} /* end of rtk_l2_portAgingEnable_set */

/* Function Name:
 *      rtk_l2_trkAgingEnable_get
 * Description:
 *      Get the dynamic address aging out state configuration of the specified trunk.
 * Input:
 *      unit    - unit id
 *      trunk   - trunk id
 * Output:
 *      pEnable - enable status of aging out
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_trkAgingEnable_get(uint32 unit, rtk_trk_t trunk, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_trkAgingEnable_get(unit, trunk, pEnable);
} /* end of rtk_l2_trkAgingEnable_get */

/* Function Name:
 *      rtk_l2_trkAgingEnable_set
 * Description:
 *      Set the dynamic address aging out state configuration of the specified trunk.
 * Input:
 *      unit    - unit id
 *      trunk   - trunk id
 *      enable  - enable status of aging out
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_TRUNK_ID - invalid trunk id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_trkAgingEnable_set(uint32 unit, rtk_trk_t trunk, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_trkAgingEnable_set(unit, trunk, enable);
} /* end of rtk_l2_trkAgingEnable_set */

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      rtk_l2_hashAlgo_get
 * Description:
 *      Get hash algorithm of layer2 switching.
 * Input:
 *      unit       - unit id
 * Output:
 *      pHash_algo - pointer to hash algorithm of layer2 switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_bucketHashAlgo_get(unit, bucket, pHash_algo)
 *          Parameters:
 *              bucket          -  0
 *              pHash_algo      -  pHash_algo
 */
int32
rtk_l2_hashAlgo_get(uint32 unit, uint32 *pHash_algo)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_hashAlgo_get(unit, pHash_algo);
} /* end of rtk_l2_hashAlgo_get */

/* Function Name:
 *      rtk_l2_hashAlgo_set
 * Description:
 *      Set hash algorithm of layer2 switching.
 * Input:
 *      unit      - unit id
 *      hash_algo - hash algorithm of layer2 switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The valid input is 0 and 1. 0 mean hash_algo_0; 1 mean hash_algo_1.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_bucketHashAlgo_set(unit, bucket, hash_algo)
 *          Parameters:
 *              bucket          -  0
 *              hash_algo       -  hash_algo
 */
int32
rtk_l2_hashAlgo_set(uint32 unit, uint32 hash_algo)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_hashAlgo_set(unit, hash_algo);
} /* end of rtk_l2_hashAlgo_set */
#endif

/* Function Name:
 *      rtk_l2_bucketHashAlgo_get
 * Description:
 *      Get bucket hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 *      bucket      - bucket id
 * Output:
 *      pHash_algo  - pointer to hash algorithm of layer2 switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      bucket can be 0 to 1.
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_l2_hashAlgo_get
 */
int32
rtk_l2_bucketHashAlgo_get(uint32 unit, uint32 bucket, uint32 *pHash_algo)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_bucketHashAlgo_get(unit, bucket, pHash_algo);
} /* end of rtk_l2_bucketHashAlgo_get */

/* Function Name:
 *      rtk_l2_bucketHashAlgo_set
 * Description:
 *      Set bucket hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 *      bucket      - bucket id
 *      hash_algo   - hash algorithm of layer2 switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The valid bucket is 0 to 1, and hash_algo is 0 and 1
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_l2_hashAlgo_set
 */
int32
rtk_l2_bucketHashAlgo_set(uint32 unit, uint32 bucket, uint32 hash_algo)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_bucketHashAlgo_set(unit, bucket, hash_algo);
} /* end of rtk_l2_bucketHashAlgo_set */

/* Function Name:
 *      rtk_l2_vlanMode_get
 * Description:
 *      Get vlan(inner/outer vlan) for L2 lookup on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pVlanMode - pointer to inner/outer vlan
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Vlan mode is as following
 *          - BASED_ON_INNER_VLAN
 *          - BASED_ON_OUTER_VLAN
 *      (2) Change to use rtk_vlan_portFwdVlan_get(unit, port, tagMode, pVlanType) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_l2_vlanMode_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pVlanMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_vlanMode_get(unit, port, pVlanMode);

} /* end of rtk_l2_vlanMode_get */

/* Function Name:
 *      rtk_l2_vlanMode_set
 * Description:
 *      Set vlan(inner/outer vlan) for L2 lookup on specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      vlanMode - inner/outer vlan
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Vlan mode is as following
 *          - BASED_ON_INNER_VLAN
 *          - BASED_ON_OUTER_VLAN
 *      (2) Change to use rtk_vlan_portFwdVlan_set(unit, port, tagMode, vlanType) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_l2_vlanMode_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t vlanMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_vlanMode_set(unit, port, vlanMode);

} /* end of rtk_l2_vlanMode_set */

/* Module Name    : L2      */
/* Sub-module Name: Unicast */

/* Function Name:
 *      rtk_l2_learningFullAction_get
 * Description:
 *      Get the forwarding action which is taken when SA learning full.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_learningFullAction_get(uint32 unit, rtk_action_t *pAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_learningFullAction_get(unit, pAction);
} /* end of rtk_l2_learningFullAction_get */

/* Function Name:
 *      rtk_l2_learningFullAction_set
 * Description:
 *      Set the forwarding action which is taken when SA learning full.
 * Input:
 *      unit   - unit id
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_learningFullAction_set(uint32 unit, rtk_action_t action)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_learningFullAction_set(unit, action);
} /* end of rtk_l2_learningFullAction_set */

/* Function Name:
 *      rtk_l2_portNewMacOp_get
 * Description:
 *      Get learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pLrnMode   - pointer to learning mode
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Forwarding action is as following
 *          - ACTION_FORWARD
 *          - ACTION_DROP
 *          - ACTION_TRAP2CPU
 *          - ACTION_COPY2CPU
 *          - ACTION_TRAP2MASTERCPU
 *          - ACTION_COPY2MASTERCPU
 *
 *      (2) Learning mode is as following
 *          - HARDWARE_LEARNING
 *          - SOFTWARE_LEARNING
 *          - NOT_LEARNING
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_portNewMacOp_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  *pLrnMode,
    rtk_action_t            *pFwdAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portNewMacOp_get(unit, port, pLrnMode, pFwdAction);

} /* end of l2_portNewMacOp_get */

/* Function Name:
 *      rtk_l2_portNewMacOp_set
 * Description:
 *      Set learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      lrnMode   - learning mode
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_INPUT      - invalid input parameter
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Forwarding action is as following
 *          - ACTION_FORWARD
 *          - ACTION_DROP
 *          - ACTION_TRAP2CPU
 *          - ACTION_COPY2CPU
 *          - ACTION_TRAP2MASTERCPU
 *          - ACTION_COPY2MASTERCPU
 *
 *      (2) Learning mode is as following
 *          - HARDWARE_LEARNING
 *          - SOFTWARE_LEARNING
 *          - NOT_LEARNING
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_portNewMacOp_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  lrnMode,
    rtk_action_t            fwdAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portNewMacOp_set(unit, port, lrnMode, fwdAction);

} /* end of l2_portNewMacOp_get */

/* Module Name    : L2              */
/* Sub-module Name: Unicast address */

/* Function Name:
 *      rtk_l2_addr_init
 * Description:
 *      Initialize content of buffer of L2 entry.
 *      Will fill vid ,MAC address and reset other field of L2 entry.
 * Input:
 *      unit     - unit id
 *      vid      - vlan id
 *      pMac     - MAC address
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Need to initialize L2 entry before add it.
 * Changes:
 *      None
 */
int32
rtk_l2_addr_init(
    uint32              unit,
    rtk_vlan_t          vid,
    rtk_mac_t           *pMac,
    rtk_l2_ucastAddr_t  *pL2_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_addr_init(unit, vid, pMac, pL2_addr);
} /* end of rtk_l2_addr_init */

/* Function Name:
 *      rtk_l2_addr_add
 * Description:
 *      Add L2 entry to ASIC.
 * Input:
 *      unit     - unit id
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_DEV_ID       - invalid device id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The API can supported add by port or trunk-id view both.
 *          - If pL2_addr->flags have turn on the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *            pL2_addr->trk_gid is valid and pL2_addr->port is invalid.
 *          - If pL2_addr->flags have turn off the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *            pL2_addr->port is valid and pL2_addr->trk_gid is invalid.
 *      (2) When adding a entry with RTK_L2_UCAST_FLAG_SA_BLOCK or RTK_L2_UCAST_FLAG_SA_SECURE attribute,
 *          pL2_addr->port should be set to 0x3f in 8390 and 9310, and 0x1f in 8380 and 9300 if the specified MAC address
 *          would come from nowhere.
 * Changes:
 *      [SDK_3.0.0]
 *          Provides the replaced mac information when it is replaced by RTK_L2_ADD_OP_FLAG_REPLACE_DYNAMIC.
 */
int32
rtk_l2_addr_add(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_addr_add(unit, pL2_addr);

} /* end of rtk_l2_addr_add */

/* Function Name:
 *      rtk_l2_addr_del
 * Description:
 *      Delete a L2 unicast address entry from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      pMac - mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      VID is same as FID in IVL mode.
 * Changes:
 *      None
 */
int32
rtk_l2_addr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_addr_del(unit, vid, pMac);
} /* end of rtk_l2_addr_del */

/* Function Name:
 *      rtk_l2_addr_get
 * Description:
 *      Get L2 entry based on specified vid and MAC address
 * Input:
 *      unit     - unit id
 * Output:
 *      pL2_addr - pointer to L2 entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) VID is same as FID in IVL mode.
 *      (2) The pL2_data.vid and pL2_data.mac is input key
 *      (3) The pL2_data.port, pL2_data.auth, pL2_data.sa_block,
 *          pL2_data.da_block and pL2_data.is_static is output.
 *      (4) If pL2_addr->flags have turn on the RTK_L2_UCAST_FLAG_TRUNK_PORT flag,
 *          mean the pL2_addr->trk_gid is valid and pL2_addr->port is valid also.
 *          The pL2_addr->port value is the represent port of pL2_addr->trk_gid.
 *      (5) If pL2_addr->flags have turn off the RTK_L2_UCAST_FLAG_TRUNK_PORT flag,
 *          mean the pL2_addr->port is valid and pL2_addr->trk_gid is invalid.
 * Changes:
 *      None
 */
int32
rtk_l2_addr_get(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_addr_get(unit, pL2_addr);

}/* end of rtk_l2_addr_get */

/* Function Name:
 *      rtk_l2_addr_set
 * Description:
 *      Update content of L2 entry.
 * Input:
 *      unit     - unit id
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_DEV_ID       - invalid device id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The API can support adding entry by port or trunk-id view both.
 *          - If pL2_addr->flags have turn on the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *            pL2_addr->trk_gid is valid and pL2_addr->port is invalid.
 *          - If pL2_addr->flags have turn off the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *            pL2_addr->port is valid and pL2_addr->trk_gid is invalid.
 * Changes:
 *      None
 */
int32
rtk_l2_addr_set(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_addr_set(unit, pL2_addr);

}/* end of rtk_l2_addr_set */

/* Function Name:
 *      rtk_l2_addr_delAll
 * Description:
 *      Delete all L2 unicast address entry from the specified device.
 * Input:
 *      unit           - unit id
 *      include_static - include static mac or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l2_addr_delAll(uint32 unit, uint32 include_static)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_addr_delAll(unit, include_static);
} /* end of rtk_l2_addr_delAll */

/* Function Name:
 *      rtk_l2_nextValidAddr_get
 * Description:
 *      Get next valid L2 unicast address entry from the specified device.
 * Input:
 *      unit           - unit id
 *      pScan_idx      - currently scan index of l2 table to get next.
 *      include_static - the get type, include static mac or not.
 * Output:
 *      pL2_data       - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          returned next valid L2 unicast address which based on index order of l2 table.
 *      (2) Input -1 for getting the first entry of l2 table.
 *      (3) The pScan_idx is both the input and output argument.
 * Changes:
 *      None
 */
int32
rtk_l2_nextValidAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    uint32              include_static,
    rtk_l2_ucastAddr_t  *pL2_data)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_nextValidAddr_get(unit, pScan_idx, include_static, pL2_data);

}/* end of rtk_l2_addr_get */

/* Module Name    : L2           */
/* Sub-module Name: l2 multicast */

/* Function Name:
 *      rtk_l2_mcastAddr_init
 * Description:
 *      Initialize content of buffer of L2 multicast entry.
 *      Will fill vid ,MAC address and reset other field of L2 multicast entry.
 * Input:
 *      unit        - unit id
 *      vid         - vlan id
 *      pMac        - MAC address
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Need to initialize L2 multicast entry before add it.
 * Changes:
 *      None
 */
int32
rtk_l2_mcastAddr_init(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac, rtk_l2_mcastAddr_t *pMcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastAddr_init(unit, vid, pMac, pMcast_addr);
} /* end of rtk_l2_mcastAddr_init */

/* Function Name:
 *      rtk_l2_mcastAddr_add
 * Description:
 *      Add L2 multicast entry to ASIC.
 * Input:
 *      unit        - unit id
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Don't need to configure pMcast_addr->fwdIndex because driver automatically allocates a
 *          free portmask entry index and return it back to pMcast_addr->fwdIndex.
 *      (2) pMcast_addr->portmask is used to configure the allocated portmask entry.
 * Changes:
 *      None
 */
int32
rtk_l2_mcastAddr_add(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastAddr_add(unit, pMcast_addr);

} /* end of rtk_l2_mcastAddr_add */

/* Function Name:
 *      rtk_l2_mcastAddr_del
 * Description:
 *      Delete a L2 multicast address entry from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      pMac - multicast mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_VLAN_VID       - invalid vlan id
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_MAC            - invalid mac address
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The corresponding portmask entry is cleared only if its reference count reaches 0.
 * Changes:
 *      None
 */
int32
rtk_l2_mcastAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastAddr_del(unit, vid, pMac);
} /* end of rtk_l2_mcastAddr_del */

/* Function Name:
 *      rtk_l2_mcastAddr_get
 * Description:
 *      Get L2 multicast entry based on specified VID and MAC address.
 * Input:
 *      unit        - unit id
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l2_mcastAddr_get(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastAddr_get(unit, pMcast_addr);

} /* end of rtk_l2_mcastAddr_add */

/* Function Name:
 *      rtk_l2_mcastAddr_set
 * Description:
 *      Update content of L2 multicast entry.
 * Input:
 *      unit        - unit id
 * Output:
 *      pMcast_addr - pointer to L2 multicast entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l2_mcastAddr_set(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastAddr_set(unit, pMcast_addr);

} /* end of rtk_l2_mcastAddr_add */

/* Function Name:
 *      rtk_l2_mcastAddr_addByIndex
 * Description:
 *      Add a L2 multicast address entry and multicast index to the specified device.
 * Input:
 *      unit                   - unit id
 *      vid                    - vlan id
 *      pMcast_addr            - content of L2 multicast address entry
 *      pMacast_addr->fwdIndex - index of portmask entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_L2_HASH_KEY        - invalid L2 Hash key
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_VLAN_VID           - invalid vlan id
 *      RT_ERR_MAC                - invalid mac address
 *      RT_ERR_L2_MULTI_FWD_INDEX - invalid index of multicast forwarding portmask
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) If pMcast_addr->fwdIndex is larger than or equal to 0, it is used for pointing to portmask entry.
 *          In this case, the portmask entry should be configured through rtk_l2_mcastFwdPortmask_set
 *          and pMcast_addr->portmask is not used.
 *
 *      (2) If pMcast_addr->fwdIndex is smaller than 0, driver automatically allocates a free portmask entry index
 *          and return it back to pMcast_addr->fwdIndex. In this case, pMcast_addr->portmask is used to
 *          configure the allocated portmask entry.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_l2_mcastAddr_add_with_index)
 */
int32
rtk_l2_mcastAddr_addByIndex(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastAddr_addByIndex(unit, pMcast_addr);

} /* end of rtk_l2_mcastAddr_addByIndex */

/* Function Name:
 *      rtk_l2_mcastAddr_setByIndex
 * Description:
 *      Update content of L2 multicast entry.
 * Input:
 *      unit        - unit id
 * Output:
 *      pMcast_addr - pointer to L2 multicast entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l2_mcastAddr_setByIndex(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastAddr_setByIndex(unit, pMcast_addr);
}

/* Function Name:
 *      rtk_l2_mcastAddr_delIgnoreIndex
 * Description:
 *      Delete a L2 multicast address entry from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      pMac - multicast mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_VLAN_VID       - invalid vlan id
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_MAC            - invalid mac address
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l2_mcastAddr_delIgnoreIndex(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastAddr_delIgnoreIndex(unit, vid, pMac);
}

/* Function Name:
 *      rtk_l2_nextValidMcastAddr_get
 * Description:
 *      Get next valid L2 multicast address entry from the specified device.
 * Input:
 *      unit      - unit id
 *      pScan_idx - currently scan index of l2 table to get next.
 * Output:
 *      pL2_data  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The function will skip valid l2 unicast and ip multicast entry and
 *          returned next valid L2 multicast address which based on index order of l2 table.
 *      (2) Input -1 for getting the first entry of l2 table.
 *      (3) The pScan_idx is both the input and output argument.
 * Changes:
 *      None
 */
int32
rtk_l2_nextValidMcastAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    rtk_l2_mcastAddr_t  *pL2_data)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_nextValidMcastAddr_get(unit, pScan_idx, pL2_data);

} /* end of rtk_l2_nextValidMcastAddr_get */

/* Module Name    : L2           */
/* Sub-module Name: IP multicast */

/* Function Name:
 *      rtk_l2_ipmcMode_get
 * Description:
 *      Get lookup mode of layer2 ip multicast switching.
 * Input:
 *      unit  - unit id
 * Output:
 *      pMode - pointer to lookup mode of layer2 ip multicast switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Lookup mode of layer2 ip multicast switching is as following
 *          - LOOKUP_ON_FVID_AND_MAC
 *          - LOOKUP_ON_DIP_AND_SIP
 *          - LOOKUP_ON_DIP_AND_FVID
 *      (2) Change to use rtk_vlan_profile_get(unit, idx, pProfile) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_l2_ipmcMode_get(uint32 unit, rtk_l2_ipmcMode_t *pMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipmcMode_get(unit, pMode);
} /* end of rtk_l2_ipmcMode_get */

/* Function Name:
 *      rtk_l2_ipmcMode_set
 * Description:
 *      Set lookup mode of layer2 ip multicast switching.
 * Input:
 *      unit - unit id
 *      mode - lookup mode of layer2 ip multicast switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Lookup mode of layer2 ip multicast switching is as following
 *          - LOOKUP_ON_FVID_AND_MAC
 *          - LOOKUP_ON_DIP_AND_SIP
 *          - LOOKUP_ON_DIP_AND_FVID
 *      (2) Change to use rtk_vlan_profile_set(unit, idx, pProfile) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_l2_ipmcMode_set(uint32 unit, rtk_l2_ipmcMode_t mode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipmcMode_set(unit, mode);
} /* end of rtk_l2_ipmcMode_set */

/* Function Name:
 *      rtk_l2_ipMcastAddrExt_init
 * Description:
 *      Initialize content of buffer of IP multicast entry.
 *      Will destination IP ,source IP and reset other field of IP multicast entry.
 * Input:
 *      unit                - unit id
 *      pIpMcast_hashKey    - the hash key to initialize content of buffer
 *      pIpMcast_addr       - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_IPV4_ADDRESS - Invalid IPv4 address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      Need to initialize IP multicast entry before add it.
 * Changes:
 *      None
 */
int32
rtk_l2_ipMcastAddrExt_init(uint32 unit, rtk_l2_ipMcastHashKey_t *pIpMcast_hashKey, rtk_l2_ipMcastAddr_t *pIpMcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipMcastAddrExt_init(unit, pIpMcast_hashKey, pIpMcast_addr);
}

/* Function Name:
 *      rtk_l2_ipMcastAddr_add
 * Description:
 *      Add IP multicast entry to ASIC.
 * Input:
 *      unit          - unit id
 *      pIpmcast_addr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_IPV4_ADDRESS       - Invalid IPv4 address
 *      RT_ERR_VLAN_VID           - invalid vlan id
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX - invalid index of multicast forwarding portmask
 *      RT_ERR_L2_ENTRY_EXIST     - the entry is already existed
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ipmcMode_set for switching the IP multicast lookup mode.
 *      (2) Don't need to configure pIpmcast_addr->fwdIndex because driver automatically allocates a
 *          free portmask entry index and return it back to pIpmcast_addr->fwdIndex.
 *      (3) pIpmcast_addr->portmask is used to configure the allocated portmask entry.
 *      (4) Change to use
 *              rtk_mcast_group_create(unit, flags, type, pGroup)
 *              rtk_ipmc_addr_add(unit, pAddr)
 *              rtk_mcast_egrIf_add(unit, group, pEgrIf)
 *          for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_l2_ipMcastAddr_add(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipMcastAddr_add(unit, pIpmcast_addr);

} /* end of rtk_l2_ipMcastAddr_add */

/* Function Name:
 *      rtk_l2_ipMcastAddr_del
 * Description:
 *      Delete a L2 ip multicast address entry from the specified device.
 * Input:
 *      unit - unit id
 *      sip  - source ip address
 *      dip  - destination ip address
 *      vid  - vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_L2_HASH_KEY            - invalid L2 Hash key
 *      RT_ERR_VLAN_VID               - invalid vlan id
 *      RT_ERR_L2_EMPTY_ENTRY         - the entry is empty(invalid)
 *      RT_ERR_L2_IPMCAST_LOOKUP_MODE - invalid IP multicast lookup mode
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ipmcMode_set for switching the IP multicast lookup mode.
 *      (2) The corresponding portmask entry is cleared only if its reference count reaches 0.
 *      (3) Change to use
 *              rtk_mcast_egrIf_del(unit, group, pEgrIf)
 *              rtk_ipmc_addr_del(unit, pAddr)
 *              rtk_mcast_group_del(unit, flags, type, pGroup)
 *          for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_l2_ipMcastAddr_del(uint32 unit, ipaddr_t sip, ipaddr_t dip, rtk_vlan_t vid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipMcastAddr_del(unit, sip, dip, vid);
} /* end of rtk_l2_ipMcastAddr_del */

/* Function Name:
 *      rtk_l2_ipMcastAddr_get
 * Description:
 *      Get IPv4 multicast entry based on specified hash key.
 * Input:
 *      unit          - unit id
 *      pIpmcast_addr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_IPV4_ADDRESS - Invalid IPv4 address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ipmcMode_set for switching the IP multicast lookup mode.
 * Changes:
 *      None
 */
int32
rtk_l2_ipMcastAddr_get(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipMcastAddr_get(unit, pIpmcast_addr);

} /* end of rtk_l2_ipMcastAddr_get */

/* Function Name:
 *      rtk_l2_ipMcastAddr_set
 * Description:
 *      Update content of IP multicast entry.
 * Input:
 *      unit          - unit id
 *      pIpmcast_addr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_IPV4_ADDRESS           - Invalid IPv4 address
 *      RT_ERR_VLAN_VID               - invalid vlan id
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_INPUT                  - invalid input parameter
 *      RT_ERR_L2_IPMCAST_LOOKUP_MODE - invalid IP multicast lookup mode
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ipmcMode_set for switching the IP multicast lookup mode.
 * Changes:
 *      None
 */
int32
rtk_l2_ipMcastAddr_set(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipMcastAddr_set(unit, pIpmcast_addr);

} /* end of rtk_l2_ipMcastAddr_set */

/* Function Name:
 *      rtk_l2_ipMcastAddr_addByIndex
 * Description:
 *      Add a IP multicast address entry and multicast index to the specified device.
 * Input:
 *      unit                     - unit id
 *      pIpMcast_addr            - content of IP multicast address entry
 * Output:
 *      pIpMacast_addr->fwdIndex - index of multicast forwarding entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_L2_HASH_KEY            - invalid L2 Hash key
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX     - invalid index of multicast forwarding portmask
 *      RT_ERR_L2_IPMCAST_LOOKUP_MODE - invalid IP multicast lookup mode
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ipmcMode_set for switching the IP multicast lookup mode.
 *      (2) If pIpMcast_addr->fwdIndex is larger than or equal to 0, it is used for pointing to portmask entry.
 *          In this case, the portmask entry should be configured through rtk_l2_mcastFwdPortmask_set
 *          and pIpMcast_addr->portmask is not used.
 *      (3) If pIpMcast_addr->fwdIndex is smaller than 0, driver automatically allocates a free portmask entry index
 *          and return it back to pIpMcast_addr->fwdIndex. In this case, pIpMcast_addr->portmask is used to
 *          configure the allocated portmask entry.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_l2_ipMcastAddr_add_with_index)
 */
int32
rtk_l2_ipMcastAddr_addByIndex(uint32 unit, rtk_l2_ipMcastAddr_t *pIpMcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipMcastAddr_addByIndex(unit, pIpMcast_addr);

} /* end of rtk_l2_ipMcastAddr_addByIndex */

/* Function Name:
 *      rtk_l2_ipMcastAddr_setByIndex
 * Description:
 *      Update content of IP multicast entry.
 * Input:
 *      unit          - unit id
 *      pIpmcast_addr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_IPV4_ADDRESS           - Invalid IPv4 address
 *      RT_ERR_VLAN_VID               - invalid vlan id
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_INPUT                  - invalid input parameter
 *      RT_ERR_L2_IPMCAST_LOOKUP_MODE - invalid IP multicast lookup mode
 * Applicable:
 *      8380
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ipmcMode_set for switching the IP multicast lookup mode.
 * Changes:
 *      None
 */
int32
rtk_l2_ipMcastAddr_setByIndex(uint32 unit, rtk_l2_ipMcastAddr_t *pIpmcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipMcastAddr_setByIndex(unit, pIpmcast_addr);
}

/* Function Name:
 *      rtk_l2_ipMcastAddr_delIgnoreIndex
 * Description:
 *      Delete a L2 ip multicast address entry from the specified device.
 * Input:
 *      unit - unit id
 *      sip  - source ip address
 *      dip  - destination ip address
 *      vid  - vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_L2_HASH_KEY            - invalid L2 Hash key
 *      RT_ERR_VLAN_VID               - invalid vlan id
 *      RT_ERR_L2_EMPTY_ENTRY         - the entry is empty(invalid)
 *      RT_ERR_L2_IPMCAST_LOOKUP_MODE - invalid IP multicast lookup mode
 * Applicable:
 *      8380
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ipmcMode_set for switching the IP multicast lookup mode.
 *      (2) Change to use
 *              rtk_mcast_egrIf_del(unit, group, pEgrIf)
 *              rtk_ipmc_addr_del(unit, pAddr)
 *              rtk_mcast_group_del(unit, flags, type, pGroup)
 *          for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_l2_ipMcastAddr_delIgnoreIndex(uint32 unit, ipaddr_t sip, ipaddr_t dip, rtk_vlan_t vid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipMcastAddr_delIgnoreIndex(unit, sip, dip, vid);
}


/* Function Name:
 *      rtk_l2_nextValidIpMcastAddr_get
 * Description:
 *      Get next valid L2 ip multicast address entry from the specified device.
 * Input:
 *      unit      - unit id
 *      pScan_idx - currently scan index of l2 table to get next
 * Output:
 *      pL2_data  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) The function will skip valid l2 unicast and multicast entry and
 *          returned next valid L2 ip multicast address which based on index order of l2 table.
 *      (2) Input -1 for getting the first entry of l2 table.
 *      (3) The pScan_idx is both the input and output argument.
 * Changes:
 *      None
 */
int32
rtk_l2_nextValidIpMcastAddr_get(
    uint32                  unit,
    int32                   *pScan_idx,
    rtk_l2_ipMcastAddr_t    *pL2_data)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_nextValidIpMcastAddr_get(unit, pScan_idx, pL2_data);

} /* end of rtk_l2_nextValidIpMcastAddr_get */

/* Function Name:
 *      rtk_l2_ipMcastAddrChkEnable_get
 * Description:
 *      Get DIP check status for IPv4 multicast packet.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to DIP check status for IPv4 multicast packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Enable the function to qualify the DIP. If the function is disabled, the packet is deemed to be
 *          IPv4 multicast packet only if DMAC[40]=1 & DIP[31:28]=1110; if enabled, the packet is deemed to be
 *          IPv4 multicast packet only if DMAC[40]=1 & DIP[31:28]=1110 and DIP[22:0] = DMAC[22:0].
 *      (2) Not Support for 9300,9310, this feature is always ENABLED
 * Changes:
 *      None
 */
int32
rtk_l2_ipMcastAddrChkEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipMcastAddrChkEnable_get(unit, pEnable);
} /* end of rtk_l2_ipMcastAddrChkEnable_get */

/* Function Name:
 *      rtk_l2_ipMcastAddrChkEnable_set
 * Description:
 *      Set DIP check status for IPv4 multicast packet.
 * Input:
 *      unit   - unit id
 *      enable - DIP check status for IPv4 multicast packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Enable the function to qualify the DIP. If the function is disabled, the packet is deemed to be
 *          IPv4 multicast packet only if DMAC[40]=1 & DIP[31:28]=1110; if enabled, the packet is deemed to be
 *          IPv4 multicast packet only if DMAC[40]=1 & DIP[31:28]=1110 and DIP[22:0] = DMAC[22:0].
 *      (2) Not Support for 9300,9310, this feature is always ENABLED
 * Changes:
 *      None
 */
int32
rtk_l2_ipMcastAddrChkEnable_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipMcastAddrChkEnable_set(unit, enable);
} /* end of rtk_l2_ipMcastAddrChkEnable_set */

/* Function Name:
 *      rtk_l2_ipMcstFidVidCompareEnable_get
 * Description:
 *      Get VID/FID comparison configuration of IP multicast entry.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to FID/VID comparison status for IPv4 multicast packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      While IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP, in addition to use SIP and DIP to
 *      lookup the IP multicast packet, VID/FID can also be used together to doing lookup through the API.
 * Changes:
 *      None
 */
int32
rtk_l2_ipMcstFidVidCompareEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipMcstFidVidCompareEnable_get(unit, pEnable);
} /* end of rtk_l2_ipMcstFidVidCompareEnable_get */

/* Function Name:
 *      rtk_l2_ipMcstFidVidCompareEnable_set
 * Description:
 *      Set VID/FID comparison configuration of IP multicast entry.
 * Input:
 *      unit   - unit id
 *      enable - configure value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      While IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP, in addition to use SIP and DIP to
 *      lookup the IP multicast packet, VID/FID can also be used to doing lookup through the API.
 * Changes:
 *      None
 */
int32
rtk_l2_ipMcstFidVidCompareEnable_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ipMcstFidVidCompareEnable_set(unit, enable);
} /* end of rtk_l2_ipMcstFidVidCompareEnable_set */

/* Function Name:
 *      rtk_l2_ip6mcMode_get
 * Description:
 *      Get lookup mode of layer2 IPv6 multicast switching.
 * Input:
 *      unit  - unit id
 * Output:
 *      pMode - pointer to lookup mode of layer2 IPv6 multicast switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Lookup mode of layer2 ip multicast switching is as following
 *          - LOOKUP_ON_FVID_AND_MAC
 *          - LOOKUP_ON_DIP_AND_SIP
 *          - LOOKUP_ON_DIP_AND_FVID
 *      (2) Change to use rtk_vlan_profile_get(unit, idx, pProfile) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_l2_ip6mcMode_get(uint32 unit, rtk_l2_ipmcMode_t *pMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ip6mcMode_get(unit, pMode);
} /* end of rtk_l2_ip6mcMode_get */

/* Function Name:
 *      rtk_l2_ip6mcMode_set
 * Description:
 *      Set lookup mode of layer2 IPv6 multicast switching.
 * Input:
 *      unit - unit id
 *      mode - lookup mode of layer2 IPv6 multicast switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Lookup mode of layer2 ip multicast switching is as following
 *          - LOOKUP_ON_FVID_AND_MAC
 *          - LOOKUP_ON_DIP_AND_SIP
 *          - LOOKUP_ON_DIP_AND_FVID
 *      (2) Change to use rtk_vlan_profile_set(unit, idx, pProfile) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_l2_ip6mcMode_set(uint32 unit, rtk_l2_ipmcMode_t mode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ip6mcMode_set(unit, mode);
} /* end of rtk_l2_ip6mcMode_set */

/* Function Name:
 *      rtk_l2_ip6CareByte_get
 * Description:
 *      Get the hash care-byte of IPv6 DIP/SIP address. These bytes are used to compose the LUT hash key.
 * Input:
 *      unit      - unit id
 *      type      - type of care-byte
 * Output:
 *      pCareByte - pointer to the care-byte of IPv6 DIP/SIP address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) careByte = 0x3210, menas take IPv6 IP address byte 3,2,1,0 to compose the hash key.
 *      (2) Byte ordering: 2001:DB8:2de::e13, byte[15]='20', byte[0]='13'.
 * Changes:
 *      None
 */
int32
rtk_l2_ip6CareByte_get(uint32 unit, rtk_l2_ip6_careByte_type_t type, uint32 *pCareByte)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ip6CareByte_get(unit, type, pCareByte);
} /* end of rtk_l2_ip6CareByte_get */

/* Function Name:
 *      rtk_l2_ip6CareByte_set
 * Description:
 *      Set the hash care-byte of IPv6 DIP/SIP address. These bytes are used to compose the LUT hash key.
 * Input:
 *      unit     - unit id
 *      type     - type of care-byte
 *      careByte - the care-byte of IPv6 DIP/SIP address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) careByte = 0x3210, menas take IPv6 IP address byte 3,2,1,0 to compose the hash key.
 *      (2) Byte ordering: 2001:DB8:2de::e13, byte[15]='20', byte[0]='13'.
 * Changes:
 *      None
 */
int32
rtk_l2_ip6CareByte_set(uint32 unit, rtk_l2_ip6_careByte_type_t type, uint32 careByte)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ip6CareByte_set(unit, type, careByte);
} /* end of rtk_l2_ip6CareByte_set */

/* Function Name:
 *      rtk_l2_ip6McastAddrExt_init
 * Description:
 *      Initialize content of buffer of IPv6 multicast entry.
 *      Will destination IP ,source IP (or vid) and reset other field of IPv6 multicast entry.
 * Input:
 *      unit                - unit id
 *      pIp6Mcast_hashKey   - the hash key to initialize content of buffer
 *      pIp6Mcast_addr      - IPv6 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_IPV6_ADDRESS     - Invalid IPv6 address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      Need to initialize IPv6 multicast entry before add it.
 * Changes:
 *      None
 */
int32
rtk_l2_ip6McastAddrExt_init(uint32 unit, rtk_l2_ip6McastHashKey_t *pIp6Mcast_hashKey, rtk_l2_ip6McastAddr_t *pIp6Mcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ip6McastAddrExt_init(unit, pIp6Mcast_hashKey, pIp6Mcast_addr);
}

/* Function Name:
 *      rtk_l2_ip6McastAddr_add
 * Description:
 *      Add IPv6 multicast entry to ASIC.
 * Input:
 *      unit          - unit id
 *      pIpmcast_addr - IPv6 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_IPV6_ADDRESS - Invalid IPv6 address
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ip6mcMode_set for switching the IP multicast lookup mode.
 *      (2) Don't need to configure pIpmcast_addr->fwdIndex because driver automatically allocates a
 *          free portmask entry index and return it back to pIpmcast_addr->fwdIndex.
 *      (3) pIpmcast_addr->portmask is used to configure the allocated portmask entry.
 * Changes:
 *      None
 */
int32
rtk_l2_ip6McastAddr_add(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ip6McastAddr_add(unit, pIpmcast_addr);

} /* end of rtk_l2_ip6McastAddr_add */

/* Function Name:
 *      rtk_l2_ip6McastAddr_del
 * Description:
 *      Delete a L2 IPv6 multicast address entry from the specified device.
 * Input:
 *      unit - unit id
 *      sip  - source ip address
 *      dip  - destination ip address
 *      vid  - vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_L2_HASH_KEY            - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY         - the entry is empty(invalid)
 *      RT_ERR_L2_IPMCAST_LOOKUP_MODE - invalid IP multicast lookup mode
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ip6mcMode_set for switching the IP multicast lookup mode.
 *      (2) The corresponding portmask entry is cleared only if its reference count reaches 0.
 * Changes:
 *      None
 */
int32
rtk_l2_ip6McastAddr_del(uint32 unit, rtk_ipv6_addr_t sip, rtk_ipv6_addr_t dip, rtk_vlan_t vid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ip6McastAddr_del(unit, sip, dip, vid);
} /* end of rtk_l2_ip6McastAddr_del */

/* Function Name:
 *      rtk_l2_ip6McastAddr_get
 * Description:
 *      Get IPv6 multicast entry based on specified hash key.
 * Input:
 *      unit          - unit id
 *      pIpmcast_addr - IPv6 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ip6mcMode_set for switching the IP multicast lookup mode.
 * Changes:
 *      None
 */
int32
rtk_l2_ip6McastAddr_get(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ip6McastAddr_get(unit, pIpmcast_addr);

} /* end of rtk_l2_ip6McastAddr_get */

/* Function Name:
 *      rtk_l2_ip6McastAddr_set
 * Description:
 *      Update content of IPv6 multicast entry.
 * Input:
 *      unit          - unit id
 *      pIpmcast_addr - IPv6 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_VLAN_VID               - invalid vlan id
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_INPUT                  - invalid input parameter
 *      RT_ERR_L2_IPMCAST_LOOKUP_MODE - invalid IP multicast lookup mode
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ip6mcMode_set for switching the IP multicast lookup mode.
 * Changes:
 *      None
 */
int32
rtk_l2_ip6McastAddr_set(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ip6McastAddr_set(unit, pIpmcast_addr);

} /* end of rtk_l2_ip6McastAddr_set */

/* Function Name:
 *      rtk_l2_ip6McastAddr_addByIndex
 * Description:
 *      Add a IPv6 multicast entry and multicast index to the specified device.
 * Input:
 *      unit                     - unit id
 *      pIpMcast_addr            - content of IPv6 multicast address entry
 * Output:
 *      pIpMacast_addr->fwdIndex - index of multicast forwarding entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_L2_HASH_KEY            - invalid L2 Hash key
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX     - invalid index of multicast forwarding portmask
 *      RT_ERR_L2_IPMCAST_LOOKUP_MODE - invalid IP multicast lookup mode
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ip6mcMode_set for switching the IP multicast lookup mode.
 *      (2) If pIpMcast_addr->fwdIndex is larger than or equal to 0, it is used for pointing to portmask entry.
 *          In this case, the portmask entry should be configured through rtk_l2_mcastFwdPortmask_set
 *          and pIpMcast_addr->portmask is not used.
 *      (3) If pIpMcast_addr->fwdIndex is smaller than 0, driver automatically allocates a free portmask entry index
 *          and return it back to pIpMcast_addr->fwdIndex. In this case, pIpMcast_addr->portmask is used to
 *          configure the allocated portmask entry.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_l2_ip6McastAddr_add_with_index)
 */
int32
rtk_l2_ip6McastAddr_addByIndex(uint32 unit, rtk_l2_ip6McastAddr_t *pIpMcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ip6McastAddr_addByIndex(unit, pIpMcast_addr);

} /* end of rtk_l2_ip6McastAddr_addByIndex */

/* Function Name:
 *      rtk_l2_ip6McastAddr_setByIndex
 * Description:
 *      Get IPv6 multicast entry based on specified hash key.
 * Input:
 *      unit          - unit id
 *      pIpmcast_addr - IPv6 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ip6mcMode_set for switching the IP multicast lookup mode.
 * Changes:
 *      None
 */
int32
rtk_l2_ip6McastAddr_setByIndex(uint32 unit, rtk_l2_ip6McastAddr_t *pIpmcast_addr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ip6McastAddr_setByIndex(unit, pIpmcast_addr);
}

/* Function Name:
 *      rtk_l2_ip6McastAddr_delIgnoreIndex
 * Description:
 *      Delete a L2 IPv6 multicast address entry from the specified device.
 * Input:
 *      unit - unit id
 *      sip  - source ip address
 *      dip  - destination ip address
 *      vid  - vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_L2_HASH_KEY            - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY         - the entry is empty(invalid)
 *      RT_ERR_L2_IPMCAST_LOOKUP_MODE - invalid IP multicast lookup mode
 * Applicable:
 *      8380
 * Note:
 *      (1) Hash key are (vid, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_FVID.
 *          Hash key are (sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is disabled.
 *          Hash key are (vid, sip, dip) if IP multicast lookup mode is LOOKUP_ON_DIP_AND_SIP & rtk_l2_ipMcstFidVidCompareEnable_set is enabled.
 *          Refer to rtk_l2_ip6mcMode_set for switching the IP multicast lookup mode.
 * Changes:
 *      None
 */
int32
rtk_l2_ip6McastAddr_delIgnoreIndex(uint32 unit, rtk_ipv6_addr_t sip, rtk_ipv6_addr_t dip, rtk_vlan_t vid)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_ip6McastAddr_delIgnoreIndex(unit, sip, dip, vid);

}

/* Function Name:
 *      rtk_l2_nextValidIp6McastAddr_get
 * Description:
 *      Get next valid L2 IPv6 multicast entry from the specified device.
 * Input:
 *      unit      - unit id
 *      pScan_idx - currently scan index of l2 table to get next.
 * Output:
 *      pL2_data  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_VLAN_VID               - invalid vid
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND      - specified entry not found
 *      RT_ERR_L2_IPMCAST_LOOKUP_MODE - invalid IP multicast lookup mode
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) The function will skip valid l2 unicast, multicast entry and IP multicast entry and
 *          returned next valid IPv6 multicast address which based on index order of L2 table.
 *      (2) Input -1 for getting the first entry of L2 table.
 *      (3) The pScan_idx is both the input and output argument.
 * Changes:
 *      None
 */
int32
rtk_l2_nextValidIp6McastAddr_get(
    uint32                  unit,
    int32                   *pScan_idx,
    rtk_l2_ip6McastAddr_t    *pL2_data)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_nextValidIp6McastAddr_get(unit, pScan_idx, pL2_data);

} /* end of rtk_l2_nextValidIp6McastAddr_get */


/* Module Name    : L2                         */
/* Sub-module Name: Multicast forwarding table */

/* Function Name:
 *      rtk_l2_mcastFwdIndex_alloc
 * Description:
 *      Allocate index for multicast forwarding entry
 * Input:
 *      unit      - unit id
 *      pFwdIndex - pointer to index of multicast forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - The module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX       - invalid index of multicast forwarding entry
 *      RT_ERR_L2_MCAST_FWD_ENTRY_EXIST - Mcast forwarding entry already exist
 *      RT_ERR_L2_INDEXTBL_FULL         - L2 index table is full
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) If pFwdIndex is larger than or equal to 0, will use pFwdIndex as multicast index.
 *      (2) If pFwdIndex is smaller than 0, will allocate a free index and return it.
 *      (3) The reference count corresponds to the pFwdIndex is increased after a successfully allocation.
 * Changes:
 *      None
 */
int32
rtk_l2_mcastFwdIndex_alloc(
    uint32          unit,
    int32           *pFwdIndex)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastFwdIndex_alloc(unit, pFwdIndex);
} /* end of rtk_l2_mcastFwdIndex_alloc */

/* Function Name:
 *      rtk_l2_mcastFwdIndex_free
 * Description:
 *      Free index for multicast forwarding entry
 * Input:
 *      unit  - unit id
 *      index - index of multicast forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                      - invalid unit id
 *      RT_ERR_NOT_INIT                     - The module is not initial
 *      RT_ERR_L2_MULTI_FWD_INDEX           - invalid index of multicast forwarding portmask
 *      RT_ERR_L2_MCAST_FWD_ENTRY_NOT_EXIST - index of forwarding entry is not exist
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of indx is 0 ~ (multicast forwarding table size - 1)
 *      (2) The reference count corresponds to the pFwdIndex is decreased after a successfully free.
 * Changes:
 *      None
 */
int32
rtk_l2_mcastFwdIndex_free(
    uint32          unit,
    int32           index)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastFwdIndex_free(unit, index);
} /* end of rtk_l2_mcastFwdIndex_free */

/* Function Name:
 *      rtk_l2_mcastFwdIndexFreeCount_get
 * Description:
 *      Get free count of multicast forwarding entry
 * Input:
 *      unit       - unit id
 *      pFreeCount - pointer to free count of multicast forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      Not support for 9300, 9310, multicast forward table is auto allocated by API, there is no need to get free counter.
 * Changes:
 *      None
 */
int32
rtk_l2_mcastFwdIndexFreeCount_get(uint32 unit, uint32 *pFreeCount)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastFwdIndexFreeCount_get(unit, pFreeCount);
} /* end of rtk_l2_mcastFwdIndexFreeCount_get */

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      rtk_l2_mcastFwdPortmask_get
 * Description:
 *      Get portmask of multicast forwarding entry
 * Input:
 *      unit       - unit id
 *      index      - index of multicast forwarding portmask
 * Output:
 *      pPortmask  - pointer buffer of multicast ports
 *      pCrossVlan - pointer of cross vlan flag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX - invalid index of multicast forwarding portmask
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The valid range of indx is 0 ~ (multicast forwarding table size - 1)
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_mcastFwdPortmaskEntry_get(unit, index, pPortmask)
 *          Parameters:
 *              index          -  index
 *              pPortmask      -  pPortmask
 */
int32
rtk_l2_mcastFwdPortmask_get(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask,
    uint32          *pCrossVlan)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastFwdPortmaskEntry_get(unit, index, pPortmask);

} /* end of rtk_l2_mcastFwdPortmask_get */

/* Function Name:
 *      rtk_l2_mcastFwdPortmask_set
 * Description:
 *      Set portmask of multicast forwarding entry
 * Input:
 *      unit      - unit id
 *      index     - index of multicast forwarding portmask
 *      pPortmask - pointer buffer of multicast ports
 *      crossVlan - cross vlan flag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX - invalid index of multicast forwarding portmask
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The valid range of indx is 0 ~ (multicast forwarding table size - 1)
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_mcastFwdPortmaskEntry_set(unit, index, pPortmask)
 *          Parameters:
 *              index          -  index
 *              pPortmask      -  pPortmask
 */
int32
rtk_l2_mcastFwdPortmask_set(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask,
    uint32          crossVlan)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastFwdPortmaskEntry_set(unit, index, pPortmask);

} /* end of rtk_l2_mcastFwdPortmask_set */
#endif

/* Function Name:
 *      rtk_l2_mcastFwdPortmaskEntry_get
 * Description:
 *      Get portmask of multicast forwarding entry
 * Input:
 *      unit       - unit id
 *      index      - index of multicast forwarding portmask
 * Output:
 *      pPortmask  - pointer buffer of multicast ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX - invalid index of multicast forwarding portmask
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The valid range of indx is 0 ~ (multicast forwarding table size - 1)
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_l2_mcastFwdPortmask_get
 */
int32
rtk_l2_mcastFwdPortmaskEntry_get(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastFwdPortmaskEntry_get(unit, index, pPortmask);

} /* end of rtk_l2_mcastFwdPortmaskEntry_get */

/* Function Name:
 *      rtk_l2_mcastFwdPortmaskEntry_set
 * Description:
 *      Set portmask of multicast forwarding entry
 * Input:
 *      unit      - unit id
 *      index     - index of multicast forwarding portmask
 *      pPortmask - pointer buffer of multicast ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX - invalid index of multicast forwarding portmask
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The valid range of indx is 0 ~ (multicast forwarding table size - 1)
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_l2_mcastFwdPortmask_set
 */
int32
rtk_l2_mcastFwdPortmaskEntry_set(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_mcastFwdPortmaskEntry_set(unit, index, pPortmask);

} /* end of rtk_l2_mcastFwdPortmaskEntry_set */

/* Module Name    : L2              */
/* Sub-module Name: CPU MAC address */

/* Function Name:
 *      rtk_l2_cpuMacAddr_add
 * Description:
 *      Add a CPU mac address entry to the lookup table.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      pMac - cpu mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vid
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The packet destined to the CPU MAC is then forwarded to CPU port.
 * Changes:
 *      None
 */
int32
rtk_l2_cpuMacAddr_add(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_cpuMacAddr_add(unit, vid, pMac);
} /* end of rtk_l2_cpuMacAddr_add */

/* Function Name:
 *      rtk_l2_cpuMacAddr_del
 * Description:
 *      Delete a CPU mac address entry from the lookup table.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      pMac - cpu mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vid
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l2_cpuMacAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_cpuMacAddr_del(unit, vid, pMac);
} /* end of rtk_l2_cpuMacAddr_del */

/* Module Name    : L2        */
/* Sub-module Name: Port move */
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      rtk_l2_legalPortMoveAction_get
 * Description:
 *      Get forwarding action when legal port moving is detected on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      For 8390 and 8380, the action refer to the new port configuration.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_portMoveAction_get(unit, type, pAction)
 *          Parameters:
 *              type                        -  L2_PORT_MOVE_DYNAMIC
 *              pAction->dynAct.port        -  port
 *              pAction->dynAct.act         -  *pFwdAction
 */
int32
rtk_l2_legalPortMoveAction_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_action_t        *pFwdAction)
{
    int32 ret;
    rtk_l2_portMoveAct_t portMoveAction;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    portMoveAction.dynAct.port = port;


    if((ret = RT_MAPPER(unit)->l2_portMoveAction_get(unit, L2_PORT_MOVE_DYNAMIC, &portMoveAction)) != RT_ERR_OK)
        return ret;

    *pFwdAction = portMoveAction.dynAct.act;
    return ret;
}

/* Function Name:
 *      rtk_l2_legalPortMoveAction_set
 * Description:
 *      Set forwarding action when legal port moving is detected on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      For 8390 and 8380, the action refer to the new port configuration.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_portMoveAction_set(unit, type, pAction)
 *          Parameters:
 *              type                        -  L2_PORT_MOVE_DYNAMIC
 *              pAction->dynAct.port        -  port
 *              pAction->dynAct.act         -  fwdAction
 */
int32
rtk_l2_legalPortMoveAction_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_action_t        fwdAction)
{
    rtk_l2_portMoveAct_t portMoveAction;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    portMoveAction.dynAct.port = port;


    portMoveAction.dynAct.act = fwdAction;
    return RT_MAPPER(unit)->l2_portMoveAction_set(unit, L2_PORT_MOVE_DYNAMIC, &portMoveAction);
}

/* Function Name:
 *      rtk_l2_dynamicPortMoveForbidAction_get
 * Description:
 *      Get the forwarding action when the port moving is detected on port move forbiddance port.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer buffer of forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      The action is taken either a dynamic address entry moved out from a port move forbiddance port
 *      or to a port move forbiddance port.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_portMoveAction_get(unit, type, pAction)
 *          Parameters:
 *              type                        -  L2_PORT_MOVE_FORBID
 *              pAction->forbidAct.act      -  *pAction
 */
int32
rtk_l2_dynamicPortMoveForbidAction_get(uint32 unit, rtk_action_t *pAction)
{
    int32 ret;
    rtk_l2_portMoveAct_t portMoveAction;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    if((ret = RT_MAPPER(unit)->l2_portMoveAction_get(unit, L2_PORT_MOVE_FORBID, &portMoveAction)) != RT_ERR_OK)
        return ret;

    *pAction = portMoveAction.forbidAct.act;
    return ret;
}/* end of rtk_l2_dynamicPortMoveForbidAction_get */

/* Function Name:
 *      rtk_l2_dynamicPortMoveForbidAction_set
 * Description:
 *      Set the forwarding action when the port moving is detected on port move forbiddance port.
 * Input:
 *      unit   - unit id
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_FWD_ACTION   - invalid forwarding action
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      The action is taken either a dynamic address entry moved out from a port move forbiddance port
 *      or to a port move forbiddance port.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_l2_portMoveAction_get(unit, type, pAction)
 *          Parameters:
 *              type                        -  L2_PORT_MOVE_FORBID
 *              pAction->forbidAct.act      -  action
 */
int32
rtk_l2_dynamicPortMoveForbidAction_set(uint32 unit, rtk_action_t action)
{
    rtk_l2_portMoveAct_t portMoveAction;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    portMoveAction.forbidAct.act = action;
    return RT_MAPPER(unit)->l2_portMoveAction_set(unit, L2_PORT_MOVE_FORBID, &portMoveAction);
} /* end of rtk_l2_dynamicPortMoveForbidAction_set */
#endif

/* Function Name:
 *      rtk_l2_portMoveAction_get
 * Description:
 *      Get forwarding action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pAction  - pointer to portmove parameter
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_l2_dynamicPortMoveForbidAction_get
 *              rtk_l2_legalPortMoveAction_get
 */
int32
rtk_l2_portMoveAction_get(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveAct_t        *pAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portMoveAction_get(unit, type, pAction);

} /* end of rtk_l2_portMoveAction_get */

/* Function Name:
 *      rtk_l2_portMoveAction_set
 * Description:
 *      Set forwarding action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pAction  - pointer to portmove parameter and action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_l2_dynamicPortMoveForbidAction_set
 *              rtk_l2_legalPortMoveAction_set
 */
int32
rtk_l2_portMoveAction_set(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveAct_t        *pAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portMoveAction_set(unit, type, pAction);

} /* end of rtk_l2_portMoveAction_set */

/* Function Name:
 *      rtk_l2_portMoveLearn_get
 * Description:
 *      Get learning action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pLearn  - pointer to portmove parameter
 * Output:
 *      pLearn - pointer to learning action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_portMoveLearn_get(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveLrn_t        *pLearn)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portMoveLearn_get(unit, type, pLearn);

} /* end of rtk_l2_portMoveLearn_get */

/* Function Name:
 *      rtk_l2_portMoveLearn_set
 * Description:
 *      Set learning action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pLearn  - pointer to portmove parameter and learning action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_portMoveLearn_set(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveLrn_t        *pLearn)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portMoveLearn_set(unit, type, pLearn);

} /* end of rtk_l2_portMoveLearn_set */

/* Function Name:
 *      rtk_l2_legalPortMoveFlushAddrEnable_get
 * Description:
 *      Get the configuration of HW flush moved-port's mac of the specified device.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Enable the function to delete the port moved entry when the port moving is detected.
 *          That is, the corresponding L2 unicast entry is deleted if it is port moved.
 *      (2) Regarding the port move forwarding behavior, refer to rtk_l2_portMoveAction_set or rtk_l2_legalPortMoveAction_set (legacy).
 *      (3) The configuration applies to new port.
 * Changes:
 *      None
 */
int32
rtk_l2_legalPortMoveFlushAddrEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_legalPortMoveFlushAddrEnable_get(unit, port, pEnable);

} /* end of rtk_l2_legalPortMoveFlushAddrEnable_get */

/* Function Name:
 *      rtk_l2_legalPortMoveFlushAddrEnable_set
 * Description:
 *      Set the configuration of HW flush moved-port's mac of the specified device.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Enable the function to delete the port moved entry when the port moving is detected.
 *          That is, the corresponding L2 unicast entry is deleted if it is port moved.
 *      (2) Regarding the port move forwarding behavior, refer to rtk_l2_portMoveAction_set or rtk_l2_legalPortMoveAction_set (legacy).
 *      (3) The configuration applies to new port.
 * Changes:
 *      None
 */
int32
rtk_l2_legalPortMoveFlushAddrEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_legalPortMoveFlushAddrEnable_set(unit, port, enable);

} /* end of rtk_l2_legalPortMoveFlushAddrEnable_set */

/* Function Name:
 *      rtk_l2_staticPortMoveAction_get
 * Description:
 *      Get forwarding action when static entry port moving is detected on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Forwarding action is as following
 *          - ACTION_FORWARD
 *          - ACTION_DROP
 *          - ACTION_TRAP2CPU
 *          - ACTION_COPY2CPU
 *      (2) The configuration applies to original port.
 *      (3) Change to use rtk_l2_portMoveAction_get(unit, type, pAction) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_l2_staticPortMoveAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pFwdAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_staticPortMoveAction_get(unit, port, pFwdAction);

} /* end of rtk_l2_staticPortMoveAction_get */

/* Function Name:
 *      rtk_l2_staticPortMoveAction_set
 * Description:
 *      Set forwarding action when static entry port moving is detected on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Applicable:
 *      8380, 8390
 * Note:
 *      (1) Forwarding action is as following
 *          - ACTION_FORWARD
 *          - ACTION_DROP
 *          - ACTION_TRAP2CPU
 *          - ACTION_COPY2CPU
 *      (2) The configuration applies to original port.
 *      (3) Change to use rtk_l2_portMoveAction_set(unit, type, pAction) for 9300,9310
 * Changes:
 *      None
 */
int32
rtk_l2_staticPortMoveAction_set(uint32 unit, rtk_port_t port, rtk_action_t fwdAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_staticPortMoveAction_set(unit, port, fwdAction);

} /* end of rtk_l2_staticPortMoveAction_set */

/* Module Name    : L2                        */
/* Sub-module Name: Parameter for lookup miss */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_get
 * Description:
 *      Get flooding port mask which limits the lookup missed flooding domain.
 * Input:
 *      unit            - unit id
 *      type            - type of lookup miss
 * Output:
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_L2_PMSK_NOT_INIT - flooding portmask is not initialized
 *      RT_ERR_INPUT            - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) In 8390 and 8380, must invoke rtk_l2_lookupMissFloodPortMask_setByIndex() first.
 *      (2) In 8380, 8390, 9300 and 9310 only DLF_TYPE_UCAST and DLF_TYPE_BCAST are supported. For DLF_TYPE_MCAST,
 *          DLF_TYPE_IPMC and DLF_TYPE_IP6MC, refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 * Changes:
 *      None
 */
int32
rtk_l2_lookupMissFloodPortMask_get(uint32 unit, rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_lookupMissFloodPortMask_get(unit, type, pFlood_portmask);

} /* end of rtk_l2_lookupMissFloodPortMask_get */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_set
 * Description:
 *      Set flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      unit            - unit id
 *      type            - type of lookup miss
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) In 9300 and 9310, only DLF_TYPE_UCAST and DLF_TYPE_BCAST are supported. For DLF_TYPE_MCAST,
 *          DLF_TYPE_IPMC and DLF_TYPE_IP6MC, refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 * Changes:
 *      None
 */
int32
rtk_l2_lookupMissFloodPortMask_set(uint32 unit, rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_lookupMissFloodPortMask_set(unit, type, pFlood_portmask);

} /* end of rtk_l2_lookupMissFloodPortMask_set */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_add
 * Description:
 *      Add one port member to the lookup missed flooding port mask.
 * Input:
 *      unit       - unit id
 *      type       - type of lookup miss
 *      flood_port - flooding port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_L2_PMSK_NOT_INIT - flooding portmask is not initialized
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) In 8390 and 8380, must invoke rtk_l2_lookupMissFloodPortMask_setByIndex() first.
 *      (2) In 8380, 8390, 9300 and 9310 only DLF_TYPE_UCAST and DLF_TYPE_BCAST are supported. For DLF_TYPE_MCAST,
 *          DLF_TYPE_IPMC and DLF_TYPE_IP6MC, refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 * Changes:
 *      None
 */
int32
rtk_l2_lookupMissFloodPortMask_add(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_lookupMissFloodPortMask_add(unit, type, flood_port);

} /* end of rtk_l2_lookupMissFloodPortMask_add */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_del
 * Description:
 *      Delete one port member from the lookup missed flooding port mask.
 * Input:
 *      unit       - unit id
 *      type       - type of lookup miss
 *      flood_port - flooding port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_L2_PMSK_NOT_INIT - flooding portmask is not initialized
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) In 8390 and 8380, must invoke rtk_l2_lookupMissFloodPortMask_setByIndex() first.
 *      (2) In 8380, 8390, 9300 and 9310 only DLF_TYPE_UCAST and DLF_TYPE_BCAST are supported. For DLF_TYPE_MCAST,
 *          DLF_TYPE_IPMC and DLF_TYPE_IP6MC, refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 * Changes:
 *      None
 */
int32
rtk_l2_lookupMissFloodPortMask_del(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_lookupMissFloodPortMask_del(unit, type, flood_port);

} /* end of rtk_l2_lookupMissFloodPortMask_del */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_setByIndex
 * Description:
 *      Set flooding port mask which limits the lookup missed flooding domain.
 * Input:
 *      unit            - unit id
 *      type            - type of lookup miss
 *      idx             - index to multicast portmask table
 *      pFlood_portmask - flooding port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - The module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_INPUT                    - invalid input parameter
 *      RT_ERR_L2_MCAST_FWD_ENTRY_EXIST - Mcast forwarding entry already exist
 * Applicable:
 *      8380, 8390
 * Note:
 *      In 8390 & 8380, only DLF_TYPE_UCAST and DLF_TYPE_BCAST are supported. For DLF_TYPE_MCAST,
 *      DLF_TYPE_IPMC and DLF_TYPE_IP6MC, refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_l2_lookupMissFloodPortMask_set_with_idx)
 */
int32
rtk_l2_lookupMissFloodPortMask_setByIndex(uint32 unit, rtk_l2_lookupMissType_t type, uint32 idx, rtk_portmask_t *pFlood_portmask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_lookupMissFloodPortMask_setByIndex(unit, type, idx, pFlood_portmask);

} /* end of rtk_l2_lookupMissFloodPortMask_setByIndex */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMaskIdx_get
 * Description:
 *      Get the entry index of forwarding table that is used as unicast/broadcast flooding port mask.
 * Input:
 *      unit    - unit id
 *      type    - type of lookup miss
 * Output:
 *      pIdx    - flooding port mask configuration when unicast/multicast lookup missed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_L2_PMSK_NOT_INIT - flooding portmask is not initialized
 *      RT_ERR_INPUT            - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      In 8380, 8390, flood-portmask get/set only supports DLF_TYPE_UCAST and DLF_TYPE_BCAST. For DLF_TYPE_MCAST,
 *      DLF_TYPE_IPMC and DLF_TYPE_IP6MC, please refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 * Changes:
 *      None
 */
int32
rtk_l2_lookupMissFloodPortMaskIdx_get(uint32 unit, rtk_l2_lookupMissType_t type, uint32 *pIdx)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_lookupMissFloodPortMaskIdx_get(unit, type, pIdx);
}/* end of rtk_l2_lookupMissFloodPortMaskIdx_get */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMaskIdx_set
 * Description:
 *      Set the entry index of forwarding table that is used as unicast/broadcast flooding port mask.
 * Input:
 *      unit    - unit id
 *      type    - type of lookup miss
 *      idx     - flooding port mask configuration when unicast/multicast lookup missed.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_L2_PMSK_NOT_INIT - flooding portmask is not initialized
 *      RT_ERR_INPUT            - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      In 8380, 8390, flood-portmask get/set only supports DLF_TYPE_UCAST and DLF_TYPE_BCAST. For DLF_TYPE_MCAST,
 *      DLF_TYPE_IPMC and DLF_TYPE_IP6MC, please refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 * Changes:
 *      None
 */
int32
rtk_l2_lookupMissFloodPortMaskIdx_set(uint32 unit, rtk_l2_lookupMissType_t type, uint32 idx)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_lookupMissFloodPortMaskIdx_set(unit, type, idx);
}/* end of rtk_l2_lookupMissFloodPortMaskIdx_set */

/* Function Name:
 *      rtk_l2_portLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when destination address lookup miss.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_IP6MC
 *      - DLF_TYPE_UCAST (9300 and 9310 only support DLF_TYPE_UCAST)
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 * Changes:
 *      None
 */
int32
rtk_l2_portLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portLookupMissAction_get(unit, port, type, pAction);

} /* end of rtk_l2_portLookupMissAction_get */

/* Function Name:
 *      rtk_l2_portLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when destination address lookup miss.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      type   - type of lookup miss
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_INPUT      - invalid type of lookup miss
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_IP6MC
 *      - DLF_TYPE_UCAST (9300 and 9310 only support DLF_TYPE_UCAST)
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 * Changes:
 *      None
 */
int32
rtk_l2_portLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portLookupMissAction_set(unit, port, type, action);

} /* end of rtk_l2_portLookupMissAction_set */

/* Function Name:
 *      rtk_l2_portUcastLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when unicast destination address lookup miss.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_portUcastLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portUcastLookupMissAction_get(unit, port, pAction);

} /* end of rtk_l2_portUcastLookupMissAction_get */

/* Function Name:
 *      rtk_l2_portUcastLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when unicast destination address lookup miss.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_portUcastLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_action_t action)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portUcastLookupMissAction_set(unit, port, action);

} /* end of rtk_l2_portUcastLookupMissAction_set */


/* Module Name    : L2                 */
/* Sub-module Name: Parameter for MISC */

/* Function Name:
 *      rtk_l2_srcPortEgrFilterMask_get
 * Description:
 *      Get loopback filtering function on specified ports.
 * Input:
 *      unit             - unit id
 * Output:
 *      pFilter_portmask - ports which turn on loopback filtering function
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 * Changes:
 *      None
 */
int32
rtk_l2_srcPortEgrFilterMask_get(uint32 unit, rtk_portmask_t *pFilter_portmask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_srcPortEgrFilterMask_get(unit, pFilter_portmask);

} /* end of rtk_l2_lookupMissFloodPortMask_get */

/* Function Name:
 *      rtk_l2_srcPortEgrFilterMask_set
 * Description:
 *      Set loopback filtering function on specified ports.
 * Input:
 *      unit             - unit id
 *      pFilter_portmask - ports which turn on loopback filtering function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 * Changes:
 *      None
 */
int32
rtk_l2_srcPortEgrFilterMask_set(uint32 unit, rtk_portmask_t *pFilter_portmask)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_srcPortEgrFilterMask_set(unit, pFilter_portmask);

} /* end of rtk_l2_lookupMissFloodPortMask_set */

/* Function Name:
 *      rtk_l2_srcPortEgrFilterMask_add
 * Description:
 *      Enable the loopback filtering function on specified port.
 * Input:
 *      unit        - unit id
 *      filter_port - ports which turn on loopback filtering function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 * Changes:
 *      None
 */
int32
rtk_l2_srcPortEgrFilterMask_add(uint32 unit, rtk_port_t filter_port)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_srcPortEgrFilterMask_add(unit, filter_port);

} /* end of rtk_l2_srcPortEgrFilterMask_add */


/* Function Name:
 *      rtk_l2_srcPortEgrFilterMask_del
 * Description:
 *      Disable the loopback filtering function on specified port.
 * Input:
 *      unit        - unit id
 *      filter_port - ports which turn off loopback filtering function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 * Changes:
 *      None
 */
int32
rtk_l2_srcPortEgrFilterMask_del(uint32 unit, rtk_port_t filter_port)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_srcPortEgrFilterMask_del(unit, filter_port);

} /* end of rtk_l2_srcPortEgrFilterMask_del */

/*
 * MISC
 */

/* Function Name:
 *      rtk_l2_exceptionAddrAction_get
 * Description:
 *      Get forwarding action of packet with exception source MAC address.
 * Input:
 *      unit       - unit id
 *      exceptType - type of exception address
 * Output:
 *      pAction    - pointer to forward action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_L2_EXCEPT_ADDR_TYPE - invalid exception address type
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      For 8380, 8390, 9300 and 9310, Exception address type is as following
 *      - SA_IS_BCAST_OR_MCAST
 *      - SA_IS_ZERO
 * Changes:
 *      None
 */
int32
rtk_l2_exceptionAddrAction_get(
    uint32                          unit,
    rtk_l2_exceptionAddrType_t      exceptType,
    rtk_action_t                    *pAction)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_exceptionAddrAction_get(unit, exceptType, pAction);
} /* end of rtk_l2_exceptionAddrAction_get */

/* Function Name:
 *      rtk_l2_exceptionAddrAction_set
 * Description:
 *      Set forwarding action of packet with exception source MAC address.
 * Input:
 *      unit       - unit id
 *      exceptType - type of exception address
 *      action     - forward action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_L2_EXCEPT_ADDR_TYPE - invalid exception address type
 *      RT_ERR_INPUT               - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      For 8380, 8390, 9300 and 9310, Exception address type is as following
 *      - SA_IS_BCAST_OR_MCAST
 *      - SA_IS_ZERO
 * Changes:
 *      None
 */
int32
rtk_l2_exceptionAddrAction_set(
    uint32                          unit,
    rtk_l2_exceptionAddrType_t      exceptType,
    rtk_action_t                    action)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_exceptionAddrAction_set(unit, exceptType, action);
} /* end of rtk_l2_exceptionAddrAction_set */

/* Function Name:
 *      rtk_l2_addrEntry_get
 * Description:
 *      Get the L2 table entry by index of the specified unit.
 * Input:
 *      unit      - unit id
 *      index     - l2 table index
 * Output:
 *      pL2_entry - pointer buffer of l2 table entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The index valid range is from 0 to (L2 hash table size - 1)
 *          - 0 ~ (L2 hash table size - 1) entry in L2 hash table
 *      (2) The output entry have 2 variables (valid and entry_type) and its detail data structure
 *          - valid: 1 mean the entry is valid; 0: invalid
 *          - entry_type: FLOW_TYPE_UNICAST, FLOW_TYPE_L2_MULTI and FLOW_TYPE_IP_MULTI
 *                        the field is ignored if valid field is 0.
 *          - detail data structure is ignored if valid is 0, and its filed meanings is depended
 *            on the entry_type value.
 *      (3) If pL2_entry->flags have enabled the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *          pL2_entry->unicast.trk_gid is valid trunk id value.
 * Changes:
 *      None
 */
int32
rtk_l2_addrEntry_get(uint32 unit, uint32 index, rtk_l2_entry_t *pL2_entry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_addrEntry_get(unit, index, pL2_entry);

} /* end of rtk_l2_addrEntry_get */

/* Function Name:
 *      rtk_l2_conflictAddr_get
 * Description:
 *      Get the conflict L2 table entry from one given L2 address in the specified unit.
 * Input:
 *      unit            - unit id
 *      pL2Addr         - l2 address to find its conflict entries
 *      cfAddrList_size - buffer size of the pCfAddrList
 * Output:
 *      pCfAddrList     - pointer buffer of the conflict l2 table entry list
 *      pCf_retCnt      - return number of find conflict l2 table entry list
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The function can be used if add l2 entry return RT_ERR_L2_NO_EMPTY_ENTRY.
 *          Input the pL2Addr->entry_type and its hash key to get conflict entry information.
 *      (2) User want to prepare the return buffer pCfAddrList and via. cfAddrList_size argument
 *          tell driver its size.
 *      (3) The function will return valid L2 hash entry from the same bucket and the return number
 *          is filled in pCf_retCnt, entry data is filled in pCfAddrList.
 * Changes:
 *      None
 */
int32
rtk_l2_conflictAddr_get(
    uint32          unit,
    rtk_l2_entry_t  *pL2Addr,
    rtk_l2_entry_t  *pCfAddrList,
    uint32          cfAddrList_size,
    uint32          *pCf_retCnt)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_conflictAddr_get(unit, pL2Addr, pCfAddrList, cfAddrList_size, pCf_retCnt);

} /* end of rtk_l2_conflictAddr_get */

/* Function Name:
 *      rtk_l2_zeroSALearningEnable_get
 * Description:
 *      Get enable status of all-zero-SA learning.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l2_zeroSALearningEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_zeroSALearningEnable_get(unit, pEnable);
} /* end of rtk_l2_zeroSALearningEnable_get */

/* Function Name:
 *      rtk_l2_zeroSALearningEnable_set
 * Description:
 *      Set enable status of all-zero-SA learning.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l2_zeroSALearningEnable_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_zeroSALearningEnable_set(unit, enable);
} /* end of rtk_l2_zeroSALearningEnable_set */

/* Function Name:
 *      rtk_l2_secureMacMode_get
 * Description:
 *      Get enable status of secure source MAC address mode.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      In secure SA mode, SA lookup missed packet should be dropped. Refer to rtk_l2_newMacOp_set to
 *      drop the SA lookup missed packet.
 * Changes:
 *      None
 */
int32
rtk_l2_secureMacMode_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_secureMacMode_get(unit, pEnable);
} /* end of rtk_l2_secureMacMode_get */

/* Function Name:
 *      rtk_l2_secureMacMode_set
 * Description:
 *      Set enable status of secure source MAC address mode.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      In secure SA mode, SA lookup missed packet should be dropped. Refer to rtk_l2_newMacOp_set to
 *      drop the SA lookup missed packet.
 * Changes:
 *      None
 */
int32
rtk_l2_secureMacMode_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_secureMacMode_set(unit, enable);
} /* end of rtk_l2_secureMacMode_set */

/* Function Name:
 *      rtk_l2_portDynamicPortMoveForbidEnable_get
 * Description:
 *      Get the port moveforbiddance configuration of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled port is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 * Changes:
 *      None
 */
int32
rtk_l2_portDynamicPortMoveForbidEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portDynamicPortMoveForbidEnable_get(unit, port, pEnable);

} /* end of rtk_l2_portDynamicPortMoveForbidEnable_get */

/* Function Name:
 *      rtk_l2_portDynamicPortMoveForbidEnable_set
 * Description:
 *      Set the port move forbiddance configuration of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled port is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 * Changes:
 *      None
 */
int32
rtk_l2_portDynamicPortMoveForbidEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portDynamicPortMoveForbidEnable_set(unit, port, enable);

} /* end of rtk_l2_portDynamicPortMoveForbidEnable_set */

/* Function Name:
 *      rtk_l2_trkDynamicPortMoveForbidEnable_get
 * Description:
 *      Get the port moveforbiddance configuration of the specified trunk.
 * Input:
 *      unit    - unit id
 *      tid     - trunk id
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled trunk is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_trkDynamicPortMoveForbidEnable_get(uint32 unit, rtk_trk_t tid, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_trkDynamicPortMoveForbidEnable_get(unit, tid, pEnable);
} /* end of rtk_l2_trkDynamicPortMoveForbidEnable_get */

/* Function Name:
 *      rtk_l2_trkDynamicPortMoveForbidEnable_set
 * Description:
 *      Set the port move forbiddance configuration of the specified trunk.
 * Input:
 *      unit    - unit id
 *      tid     - trunk id
 *      enable  - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled trunk is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_trkDynamicPortMoveForbidEnable_set(uint32 unit, rtk_trk_t tid, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_trkDynamicPortMoveForbidEnable_set(unit, tid, enable);
} /* end of rtk_l2_trkDynamicPortMoveForbidEnable_set */

/* Function Name:
 *      rtk_l2_portMacFilterEnable_get
 * Description:
 *      Get the mac filter configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      filterMode  - filter DA or SA
 * Output:
 *      pEnable     - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l2_portMacFilterEnable_get(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portMacFilterEnable_get(unit, port, filterMode, pEnable);

} /* end of rtk_l2_portMacFilterEnable_get */

/* Function Name:
 *      rtk_l2_portMacFilterEnable_set
 * Description:
 *      Set the mac filter configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      filterMode  - filter DA or SA
 *      enable      - drop procedence assignment status for trapped packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l2_portMacFilterEnable_set(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portMacFilterEnable_set(unit, port, filterMode, enable);

} /* end of rtk_l2_portMacFilterEnable_set */

/* Function Name:
 *      rtk_l2_hwNextValidAddr_get
 * Description:
 *      get next valid entry with specific method.
 * Input:
 *      unit        - unit id
 *      pScan_idx   - the index which starting search from
 *      rtk_l2_nextValidType_t  - search Method
 * Output:
 *      pScan_idx           - the next valid entry index
 *      rtk_l2_entry_t      - the next valid entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_l2_hwNextValidAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    rtk_l2_nextValidType_t type,
    rtk_l2_entry_t  *pEntry)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_hwNextValidAddr_get(unit, pScan_idx, type, pEntry);

}/* end of rtk_l2_addr_get */

/* Function Name:
 *      rtk_l2_portCtrl_get
 * Description:
 *      Get the configuration of the specified control type and port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - control type
 * Output:
 *      pArg    - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_portCtrl_get(uint32 unit, rtk_port_t port, rtk_l2_portCtrlType_t type, int32 *pArg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portCtrl_get(unit, port, type, pArg);
}

/* Function Name:
 *      rtk_l2_portCtrl_set
 * Description:
 *      Set the configuration of the specified control type and port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - control type
 *      arg     - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_portCtrl_set(uint32 unit, rtk_port_t port, rtk_l2_portCtrlType_t type, int32 arg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_portCtrl_set(unit, port, type, arg);
}

/* Function Name:
 *      rtk_l2_status_get
 * Description:
 *      Get the status or counter of the specified feature
 * Input:
 *      unit    - unit id
 *      type    - status type
 *      index   - index
 * Output:
 *      pArg    - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      9300, 9310
 * Note:
 *      "index" may be only valid in some rtk_l2_stsType_t types.
 * Changes:
 *      [SDK_3.1.0]
 *          New added function.
 */
extern int32
rtk_l2_status_get(uint32 unit, rtk_l2_stsType_t type, uint32 index, uint32 *pArg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_status_get(unit, type, index, pArg);
}

/* Function Name:
 *      rtk_l2_stkLearningEnable_get
 * Description:
 *      Get the enable status of stacking system auto-learning mode.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to the enable status of stacking system auto-learning mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      When it is enabled, switch will learn the MAC and source ingress port of packet which received from stacking port.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_stkLearningEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_stkLearningEnable_get(unit, pEnable);
} /* end of rtk_l2_stkLearningEnable_get */

/* Function Name:
 *      rtk_l2_stkLearningEnable_set
 * Description:
 *      Set the enable status of stacking system auto-learning mode.
 * Input:
 *      unit    - unit id
 *      enable  - enable status of stacking system auto-learning mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Applicable:
 *      9300, 9310
 * Note:
 *      When it is enabled, switch will learn the MAC and source ingress port of packet which received from stacking port.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_stkLearningEnable_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_stkLearningEnable_set(unit, enable);
} /* end of rtk_l2_stkLearningEnable_set */

/* Function Name:
 *      rtk_l2_stkKeepUcastEntryValid_get
 * Description:
 *      Get the enable status of keeping-entry-valid in stacking system.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to the enable status of distributed lookup
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      When it is enabled, switch will keep an aging-out/link-down-flush unicast entry valid if it was not learnt on this switch.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l2_stkKeepUcastEntryValid_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_stkKeepUcastEntryValid_get(unit, pEnable);
} /* end of rtk_l2_stkKeepUcastEntryValid_get */

/* Function Name:
 *      rtk_l2_stkKeepUcastEntryValid_set
 * Description:
 *      Set the enable status of keeping-entry-valid in stacking system.
 * Input:
 *      unit    - unit id
 *      enable  - enable status of distributed lookup
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Applicable:
 *      9300, 9310
 * Note:
 *      When it is enabled, switch will keep an aging-out/link-down-flush unicast entry valid if it was not learnt on this switch.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_l2_stkKeepUcastEntryValid_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_stkKeepUcastEntryValid_set(unit, enable);
} /* end of rtk_l2_stkKeepUcastEntryValid_set */

/* Function Name:
 *      rtk_l2_entryCnt_get
 * Description:
 *      Get l2 entry counter
 * Input:
 *      unit    - unit id
 *      type    - l2 entry type
 * Output:
 *      pCnt    - pointer to the entry counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT        - invalid input
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_entryCnt_get(uint32 unit, rtk_l2_entryType_t type, uint32 *pCnt)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_entryCnt_get(unit, type, pCnt);
}   /* end of rtk_l2_entryCnt_get */

/* Function Name:
 *      rtk_l2_hashIdx_get
 * Description:
 *      Get L2 hash index based on specified vid and MAC address
 * Input:
 *      unit        - unit id
 *      pMacHashIdx - pointer to vid and mac
 * Output:
 *      pMacHashIdx - pointer to hash indexes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) VID is same as FID in IVL mode.
 *      (2) The pMacHashIdx->vid and pMacHashIdx->mac is input key
 *      (3) 8380 and 8390 support only one block, 9300, 9310 support two blocks.
 *      (4) validBlk and validAlgo fields in pMacHashIdx specify how many blocks and hash-algo number
 *          the target unit supports.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_hashIdx_get(uint32 unit, rtk_l2_macHashIdx_t *pMacHashIdx)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_hashIdx_get(unit, pMacHashIdx);

}

/* Function Name:
 *      rtk_l2_addr_delByMac
 * Description:
 *      Delete all L2 entries with specific MAC
 * Input:
 *      unit            - unit id
 *      include_static  - include static mac or not
 *      pMac            - mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_l2_addr_delByMac(uint32 unit, uint32 include_static, rtk_mac_t *pMac)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->l2_addr_delByMac(unit, include_static, pMac);
}

