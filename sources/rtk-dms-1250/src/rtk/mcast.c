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
#include <rtk/mcast.h>

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
 *      rtk_mcast_init
 * Description:
 *      Initialize multicast group module of the specified device.
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
 *      Must initialize multicast group module before calling any multicast group APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_mcast_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mcast_init(unit);
}

/* Function Name:
 *      rtk_mcast_group_create
 * Description:
 *      Create a multicast group
 * Input:
 *      unit   - unit id
 *      flags  - RTK_MCAST_FLAG_XXX
 *      type   - multicast group type
 *      pGroup - group id (use with RTK_MCAST_FLAG_WITH_ID flag)
 * Output:
 *      pGroup - pointer to multicast group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_MCAST_FLAG_WITH_ID    - to indicate the group ID use pGroup (include group type and group index)
 *                                      could use marco - RTK_MCAST_GROUP_ID(type, index) to specify it
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_mcast_group_create(uint32 unit, uint32 flags, rtk_mcast_type_t type, rtk_mcast_group_t *pGroup)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mcast_group_create(unit,flags,type,pGroup);
}

/* Function Name:
 *      rtk_mcast_group_destroy
 * Description:
 *      Destrory a multicast group
 * Input:
 *      unit  - unit id
 *      group - multicast group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
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
rtk_mcast_group_destroy(uint32 unit, rtk_mcast_group_t group)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mcast_group_destroy(unit,group);
}

/* Function Name:
 *      rtk_mcast_group_getNext
 * Description:
 *      Get the next multicast group
 * Input:
 *      unit   - unit id
 *      type   - mcast type
 *      pBase  - start index
 * Output:
 *      pGroup - pointer to multicast group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Use base index -1 to indicate to search from the beginging of group entry
 *      (2) If the returned index is -1, means not found the next valid entry
 *      (3) type = 0 indicate include all type
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_mcast_group_getNext(uint32 unit, rtk_mcast_type_t  type, int32 *pBase, rtk_mcast_group_t *pGroup)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mcast_group_getNext(unit, type, pBase, pGroup);
}   /* end of rtk_mcast_group_getNext */

/* Function Name:
 *      rtk_mcast_egrIf_get
 * Description:
 *      Get the egress interfaces information of a specified multicast group
 * Input:
 *      unit          - unit id
 *      group         - multicast group
 *      egrIf_max     - number of rtk_mcast_egrif_t entry in pEgrIf buffer
 *      pEgrIf        - buffer pointer
 * Output:
 *      *pEgrIf       - next hop information
 *      *pEgrIf_count - returned rtk_mcast_egrif_t entry number
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) pEgrIf is input buffer,user should alloc memory firstly,
 *          egrIf_max is the max entry number can use rtk_switch_deviceInfo_get()
 *          to get max_num_of_mcast_group_nexthop
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_mcast_egrIf_get(uint32 unit, rtk_mcast_group_t group, int32 egrIf_max, rtk_mcast_egrif_t *pEgrIf, int32 *pEgrIf_count)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mcast_nextHop_get(unit,group,egrIf_max,pEgrIf,pEgrIf_count);
}

/* Function Name:
 *      rtk_mcast_egrIf_add
 * Description:
 *      Add an egress interface to a multicast group replication list
 * Input:
 *      unit   - unit id
 *      group  - multicast group
 *      pEgrIf - pointer to multicast next hop information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_MASK    - invalid portmask
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Applicable flags:
 *          RTK_MCAST_EGRIF_FLAG_PMSK_REPLACE   - to indicate replace nexthop type entry portmask
 *          RTK_MCAST_EGRIF_FLAG_BRIDGE         - to indicate L3 nexthop type used for L2 bridge
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_mcast_egrIf_add(uint32 unit, rtk_mcast_group_t group, rtk_mcast_egrif_t *pEgrIf)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mcast_nextHop_add(unit,group,pEgrIf);
}

/* Function Name:
 *      rtk_mcast_egrIf_del
 * Description:
 *      Delete an egress interface from multicast group replication list
 * Input:
 *      unit   - unit id
 *      group  - multicast group
 *      pEgrIf - pointer to multicast next hop information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_MASK    - invalid portmask
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Basic required parameters of pEgrIf as input:
 *          type and type info of l2/l3/stk/tunnel.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_mcast_egrIf_del(uint32 unit, rtk_mcast_group_t group, rtk_mcast_egrif_t *pEgrIf)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mcast_nextHop_del(unit,group,pEgrIf);
}

/* Function Name:
 *      rtk_mcast_egrIf_delAll
 * Description:
 *      Delete all egress interfaces from a multicast group replication list
 * Input:
 *      unit  - unit id
 *      group - multicast group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
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
rtk_mcast_egrIf_delAll(uint32 unit, rtk_mcast_group_t group)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->mcast_nextHop_delAll(unit, group);
}

