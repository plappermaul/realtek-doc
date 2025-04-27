/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision: 80415 $
 * $Date: 2017-07-06 18:58:48 +0800 (Thu, 06 Jul 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) mcast
 *
 */

#include <dal/rtrpc/rtrpc_mcast.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

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
 */
int32
rtrpc_mcast_init(uint32 unit)
{
    rtdrv_unitCfg_t data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&data, 0, sizeof(rtdrv_unitCfg_t));
    data.unit = unit;
    SETSOCKOPT(RTDRV_MCAST_INIT, &data, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_mcast_init */

/* Function Name:
 *      rtk_mcast_group_create
 * Description:
 *      Create a multicast group
 * Input:
 *      unit  - unit id
 *      flags - RTK_MCAST_XXX
 *      type  - multicast group type
 * Output:
 *      pGroup- pointer to multicast group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 */
int32
rtrpc_mcast_group_create(uint32 unit, uint32 flags, rtk_mcast_type_t type, rtk_mcast_group_t *pGroup)
{
    rtdrv_mcastCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_mcastCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.flags, &flags, sizeof(uint32));
    osal_memcpy(&cfg.type, &type, sizeof(rtk_mcast_type_t));
    osal_memcpy(&cfg.group, pGroup, sizeof(rtk_mcast_group_t));
    GETSOCKOPT(RTDRV_MCAST_GROUP_CREATE, &cfg, rtdrv_mcastCfg_t, 1);
    osal_memcpy(pGroup, &cfg.group, sizeof(rtk_mcast_group_t));
    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_mcast_group_destroy
 * Description:
 *      Destrory the multicast group
 * Input:
 *      unit  - unit id
 *      group - multicast group
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 */

int32
rtrpc_mcast_group_destroy(uint32 unit, rtk_mcast_group_t group)
{
    rtdrv_mcastCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_mcastCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.group, &group, sizeof(rtk_mcast_group_t));
    SETSOCKOPT(RTDRV_MCAST_GROUP_DESTROY, &cfg, rtdrv_mcastCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_mcast_group_getNext
 * Description:
 *      Get next multicast group
 * Input:
 *      unit  - unit id
 *      type - mcast type
 *      pBase- start index
 * Output:
 *      pGroup - pointer to multicast group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *  *pBase = -1 indicate get first valid group
 *  type = 0 indicate include all type
 */
int32
rtrpc_mcast_group_getNext(uint32 unit, rtk_mcast_type_t  type, int32 *pBase, rtk_mcast_group_t *pGroup)
{
    rtdrv_mcastCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBase), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pGroup), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_mcastCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.type, &type, sizeof(rtk_mcast_type_t));
    osal_memcpy(&cfg.base, pBase, sizeof(int32));
    GETSOCKOPT(RTDRV_MCAST_GROUP_GETNEXT, &cfg, rtdrv_mcastCfg_t, 1);
    osal_memcpy(pBase, &cfg.base, sizeof(int32));
    osal_memcpy(pGroup, &cfg.group, sizeof(rtk_mcast_group_t));

    return RT_ERR_OK;
}   /* end of rtk_mcast_group_getNext */

/* Function Name:
 *      rtk_mcast_egrIf_get
 * Description:
 *      Get next hop information of the multicast group
 * Input:
 *      unit  - unit id
 *      group - multicast group
 *      egrIf_max - number of rtk_mcast_egrif_t entry in pEgrIf buffer
 *      pEgrIf - buffer pointer
 * Output:
 *      *pEgrIf - next hop information
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
 */
int32
rtrpc_mcast_egrIf_get(uint32 unit, rtk_mcast_group_t group, int32 egrIf_max, rtk_mcast_egrif_t *pEgrIf, int32 *pEgrIf_count)
{
    rtdrv_mcastCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEgrIf_count), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_mcastCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.group, &group, sizeof(rtk_mcast_group_t));
    osal_memcpy(&cfg.maxNum, &egrIf_max, sizeof(int32));
    cfg.pNhArry = pEgrIf;
    GETSOCKOPT(RTDRV_MCAST_NEXTHOP_GET, &cfg, rtdrv_mcastCfg_t, 1);
    osal_memcpy(pEgrIf_count, &cfg.nexthopNum,sizeof(int32));

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_mcast_egrIf_add
 * Description:
 *      Add a NH to multicast group replication list
 * Input:
 *      unit  - unit id
 *      group - multicast group
 *      pEgrIf  - pointer to multicast next hop information
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 */
int32
rtrpc_mcast_egrIf_add(uint32 unit, rtk_mcast_group_t group, rtk_mcast_egrif_t *pEgrIf)
{
    rtdrv_mcastCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_mcastCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.group, &group, sizeof(rtk_mcast_group_t));
    osal_memcpy(&cfg.nhEntry, pEgrIf, sizeof(rtk_mcast_egrif_t));
    SETSOCKOPT(RTDRV_MCAST_NEXTHOP_ADD, &cfg, rtdrv_mcastCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_mcast_egrIf_del
 * Description:
 *      Delete a NH from multicast group replication list
 * Input:
 *      unit  - unit id
 *      group - multicast group
 *      pEgrIf  - pointer to multicast next hop information
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 */
int32
rtrpc_mcast_egrIf_del(uint32 unit, rtk_mcast_group_t group, rtk_mcast_egrif_t *pEgrIf)
{
    rtdrv_mcastCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEgrIf), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_mcastCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.group, &group, sizeof(rtk_mcast_group_t));
    osal_memcpy(&cfg.nhEntry, pEgrIf, sizeof(rtk_mcast_egrif_t));
    SETSOCKOPT(RTDRV_MCAST_NEXTHOP_DEL, &cfg, rtdrv_mcastCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_mcast_egrIf_delAll
 * Description:
 *      Delete all NHs from multicast group replication list
 * Input:
 *      unit  - unit id
 *      group - multicast group
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 */

int32
rtrpc_mcast_egrIf_delAll(uint32 unit, rtk_mcast_group_t group)
{
    rtdrv_mcastCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_mcastCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.group, &group, sizeof(rtk_mcast_group_t));
    SETSOCKOPT(RTDRV_MCAST_NEXTHOP_DELALL, &cfg, rtdrv_mcastCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_mcastMapper_init(dal_mapper_t *pMapper)
{
    pMapper->mcast_group_create = rtrpc_mcast_group_create;
    pMapper->mcast_group_destroy = rtrpc_mcast_group_destroy;
    pMapper->mcast_group_getNext = rtrpc_mcast_group_getNext;
    pMapper->mcast_nextHop_get = rtrpc_mcast_egrIf_get;
    pMapper->mcast_nextHop_add = rtrpc_mcast_egrIf_add;
    pMapper->mcast_nextHop_del = rtrpc_mcast_egrIf_del;
    pMapper->mcast_nextHop_delAll = rtrpc_mcast_egrIf_delAll;
    return RT_ERR_OK;
}
