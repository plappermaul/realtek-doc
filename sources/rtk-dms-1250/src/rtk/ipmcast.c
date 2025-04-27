/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public Layer3 routing APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Layer3 routing error handling
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/l3.h>
#include <rtk/ipmcast.h>

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
 *      rtk_ipmc_init
 * Description:
 *      Initialize L3 IP multicast module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9300, 9310
 * Note:
 *      Must initialize L3 IP multicast module before calling any L3 multicast APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_ipmc_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return  RT_MAPPER(unit)->ipmc_init(unit);
}

/* Function Name:
 *      rtk_ipmc_addr_t_init
 * Description:
 *      Initialize data structure of rtk_ipmc_addr_t
 * Input:
 *      pAddr - pointer to rtk_ipmc_addr_t
 * Output:
 *      pAddr - pointer to rtk_ipmc_addr_t((initialized))
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_ipmc_addr_t_init(rtk_ipmc_addr_t *pAddr)
{
    RT_PARAM_CHK((NULL == pAddr), RT_ERR_NULL_POINTER);

    osal_memset(pAddr, 0x00, sizeof(rtk_ipmc_addr_t));
    pAddr->mtu_max = RTK_DEFAULT_L3_INTF_MTU;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_ipmc_addr_add
 * Description:
 *      Add a L3 multicast address
 * Input:
 *      unit  - unit id
 *      pAddr - pointer to L3 multicast address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found (when try to replace)
 *      RT_ERR_IPMC_ADDR      - invalid multicast ip address
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pRoute as input keys:
 *          vrf_id, src_ip_addr(if RTK_IPMC_FLAG_SIP is set) and dst_ip_addr (either ipv4 or ipv6),
 *          and group (if RTK_IPMC_FLAG_NULL_INTF isn't set).
 *          and vlan_id or intf_id for 9310(only vlan_id for 9300)
 *      (2) Applicable flags:
 *          RTK_IPMC_FLAG_REPLACE         - replace the existing entry with the new info
 *          RTK_IPMC_FLAG_NULL_INTF       - the route is destined to a null interface
 *                                          (path ID will not be referred)
 *          RTK_IPMC_FLAG_TTL_DEC_IGNORE  - ignore TTL decreasement
 *          RTK_IPMC_FLAG_TTL_CHK_IGNORE  - ignore TTL check
 *          RTK_IPMC_FLAG_QOS_ASSIGN      - assign a new internal priority (with qos_pri field)
 *          RTK_IPMC_FLAG_SIP             - entry is dip + sip
 *          RTK_IPMC_FLAG_SRC_INTF_ID     - only for 9310
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_ipmc_addr_add(uint32 unit, rtk_ipmc_addr_t *pAddr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return  RT_MAPPER(unit)->ipmc_addr_add(unit,pAddr);
}

/* Function Name:
 *      rtk_ipmc_addr_find
 * Description:
 *      Find a L3 multicast address
 * Input:
 *      unit  - unit id
 *      pAddr - pointer to L3 multicast address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pMcastAddras input keys:
 *          vrf_id and dst_ip_addr or with src_ip_addr (either ipv4 or ipv6).
 *          and vlan_id or intf_id for 9310(only vlan_id for 9300)
 *      (2) Applicable flags:
 *          RTK_IPMC_FLAG_HIT_CLEAR     - clear the hit bit if it's set
 *          RTK_IPMC_FLAG_READ_HIT_IGNORE - get entry ignore hit bit, only for 9300
 *          RTK_IPMC_FLAG_SRC_INTF_ID   - only for 9310
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_ipmc_addr_find(uint32 unit, rtk_ipmc_addr_t * pAddr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return  RT_MAPPER(unit)->ipmc_addr_find(unit,pAddr);
}

/* Function Name:
 *      rtk_ipmc_addr_del
 * Description:
 *      Delete a L3 multicast address
 * Input:
 *      unit  - unit id
 *      pAddr - pointer to L3 multicast address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pAddr input keys:
 *          vrf_id and dst_ip_addr or with src_ip_addr (either ipv4 or ipv6).
 *          and vlan_id or intf_id for 9310(only vlan_id for 9300)
 *      (2) Applicable flags:
 *          RTK_IPMC_FLAG_SRC_INTF_ID   - only for 9310
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_ipmc_addr_del(uint32 unit, rtk_ipmc_addr_t *pAddr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return  RT_MAPPER(unit)->ipmc_addr_del(unit,pAddr);
}

/* Function Name:
 *      rtk_ipmc_addr_delAll
 * Description:
 *      Delete all L3 multicast address
 * Input:
 *      unit - unit id
 *      flag - configure flag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_IPMC_FLAG_IPV6   - to indicate the entry format is IPv6 rather than IPv4
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_ipmc_addr_delAll(uint32 unit, rtk_ipmc_flag_t flag)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return  RT_MAPPER(unit)->ipmc_addr_delAll(unit,flag);
}

/* Function Name:
 *      rtk_ipmc_nextValidAddr_get
 * Description:
 *      Get next L3 multicast address with matched flags
 * Input:
 *      unit  - unit id
 *      flags - flags
 *      pBase - start index
 * Output:
 *      pAddr - pointer to L3 multicast address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_L3_FLAG_IPV6    - to indicate the entry format is IPv6 rather than IPv4
 *          RTK_IPMC_FLAG_READ_HIT_IGNORE - get entry ignore hit bit, only for 9300
 *      (2) Use base index -1 to indicate to search from the beginging of ipmcast entry table
 *      (3) If the returned index is -1, means not found the next valid entry
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_ipmc_nextValidAddr_get(uint32 unit, uint32 flags, int32 *pBase, rtk_ipmc_addr_t *pAddr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return  RT_MAPPER(unit)->ipmc_nextValidAddr_get(unit,flags,pBase,pAddr);
}

/* Function Name:
 *      rtk_ipmc_statMont_create
 * Description:
 *      Create a statistic monitor session
 * Input:
 *      unit      - unit id
 *      pStatMont - statistic monitor information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pStatMont as input keys:
 *          pStatMont->key ipmcast entry key
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_ipmc_statMont_create(uint32 unit, rtk_ipmc_statMont_t *pStatMont)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return  RT_MAPPER(unit)->ipmc_statMont_create(unit, pStatMont);
}

/* Function Name:
 *      rtk_ipmc_statMont_destroy
 * Description:
 *      Destroy a statistic monitor session
 * Input:
 *      unit      - unit id
 *      pStatMont - statistic monitor information
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required input parameters of the pStatMont as input keys:
 *          pStatMont->key ipmcast entry key
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_ipmc_statMont_destroy(uint32 unit, rtk_ipmc_statMont_t *pStatMont)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return  RT_MAPPER(unit)->ipmc_statMont_destroy(unit, pStatMont);
}

/* Function Name:
 *      rtk_ipmc_statCntr_reset
 * Description:
 *      Reset (clear) counter(s)
 * Input:
 *      unit - unit id
 *      pKey - key for search the statistic session
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_ipmc_statCntr_reset(uint32 unit, rtk_ipmc_statKey_t *pKey)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return  RT_MAPPER(unit)->ipmc_statCntr_reset(unit, pKey);
}

/* Function Name:
 *      rtk_ipmc_statCntr_get
 * Description:
 *      Get counter and parameter of L3 counter
 * Input:
 *      unit  - unit id
 *      pKey  - key for search the statistic session
 * Output:
 *      pCntr - Statistic counter vlanue
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_ipmc_statCntr_get(uint32 unit, rtk_ipmc_statKey_t *pKey, rtk_ipmc_statCntr_t *pCntr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return  RT_MAPPER(unit)->ipmc_statCntr_get(unit, pKey, pCntr);
}

/* Function Name:
 *      rtk_ipmc_globalCtrl_set
 * Description:
 *      Set status of specified L3 multicast control
 * Input:
 *      unit - unit id
 *      type - L3 multicast control type
 *      arg  - control stauts
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_ipmc_globalCtrl_set(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 arg)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return  RT_MAPPER(unit)->ipmc_globalCtrl_set(unit, type, arg);
}

/* Function Name:
 *      rtk_ipmc_globalCtrl_get
 * Description:
 *      Get status of specified L3 multicast control
 * Input:
 *      unit - unit id
 *      type - L3 multicast control type
 * Output:
 *      pArg - control stauts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_ipmc_globalCtrl_get(uint32 unit, rtk_ipmc_globalCtrlType_t type, int32 *pArg)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return  RT_MAPPER(unit)->ipmc_globalCtrl_get(unit, type, pArg);
}

