/*
 * Copyright(c) Realtek Semiconductor Corporation, 2009
 * All rights reserved.
 *
 * $Revision: 79582 $
 * $Date: 2017-06-13 16:50:29 +0800 (Tue, 13 Jun 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) ipmcast
 *
 */

#include <dal/rtrpc/rtrpc_ipmcast.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>


/* Function Name:
 *      rtk_ipmc_init
 * Description:
 *      Initialize L3 ip multicast module of the specified device.
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
 *      Must initialize L3 ip multicast module before calling any L3 multicast APIs.
 */
extern int32
rtrpc_ipmc_init(uint32 unit)
{
    rtdrv_unitCfg_t data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&data, 0, sizeof(rtdrv_unitCfg_t));
    data.unit = unit;
    SETSOCKOPT(RTDRV_IPMC_INIT, &data, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_ipmc_addr_t_init
 * Description:
 *      Initialize data structure of rtk_ipmc_addr_t
 * Input:
 *      pAddr - pointer to rtk_ipmc_addr_t
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 */
int32
rtrpc_ipmc_addr_t_init(rtk_ipmc_addr_t *pAddr)
{
    rtdrv_ipmcCfg_t cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAddr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ipmcCfg_t));
    osal_memcpy(&cfg.ipmcEntry, pAddr, sizeof(rtk_ipmc_addr_t));
    GETSOCKOPT(RTDRV_IPMC_ADDR_INIT, &cfg, rtdrv_ipmcCfg_t, 1);
    osal_memcpy(pAddr,&cfg.ipmcEntry,sizeof(rtk_ipmc_addr_t));

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_ipmc_add
 * Description:
 *      Add a L3 multicast address
 * Input:
 *      unit  - unit id
 *      pAddr - pointer to L3 multicast address
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
rtrpc_ipmc_addr_add(uint32 unit, rtk_ipmc_addr_t *pAddr)
{
    rtdrv_ipmcCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAddr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ipmcCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.ipmcEntry, pAddr, sizeof(rtk_ipmc_addr_t));
    SETSOCKOPT(RTDRV_IPMC_ADDR_ADD, &cfg, rtdrv_ipmcCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_ipmc_find
 * Description:
 *      Find a L3 multicast address
 * Input:
 *      unit  - unit id
 *      pAddr - pointer to L3 multicast address
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
rtrpc_ipmc_addr_find(uint32 unit, rtk_ipmc_addr_t * pAddr)
{
    rtdrv_ipmcCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAddr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ipmcCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.ipmcEntry, pAddr, sizeof(rtk_ipmc_addr_t));
    GETSOCKOPT(RTDRV_IPMC_ADDR_FIND, &cfg, rtdrv_ipmcCfg_t, 1);
    osal_memcpy(pAddr, &cfg.ipmcEntry, sizeof(rtk_ipmc_addr_t));

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_ipmc_del
 * Description:
 *      Delete a L3 multicast address
 * Input:
 *      unit  - unit id
 *      pAddr - pointer to L3 multicast address
 * Output:
 *      pAddr - pointer to L3 multicast address
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
rtrpc_ipmc_addr_del(uint32 unit, rtk_ipmc_addr_t *pAddr)
{
    rtdrv_ipmcCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAddr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ipmcCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.ipmcEntry, pAddr, sizeof(rtk_ipmc_addr_t));
    SETSOCKOPT(RTDRV_IPMC_ADDR_DEL, &cfg, rtdrv_ipmcCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_ipmc_delAll
 * Description:
 *      Delete all L3 multicast address
 * Input:
 *      unit  - unit id
 *      flag - configure flag
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
 *  flag only support RTK_IPMC_FLAG_IPV6
 */
int32
rtrpc_ipmc_addr_delAll(uint32 unit, rtk_ipmc_flag_t flag)
{
    rtdrv_ipmcCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ipmcCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.flags, &flag, sizeof(rtk_ipmc_flag_t));
    SETSOCKOPT(RTDRV_IPMC_ADDR_DELALL, &cfg, rtdrv_ipmcCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_ipmc_nextValidAddr_get
 * Description:
 *      Get next L3 multicast address with matched flags
 * Input:
 *      unit  - unit id
 *      flags - flags
 *      pBase- start index
 * Output:
 *      pAddr - pointer to L3 multicast address
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
rtrpc_ipmc_nextValidAddr_get(uint32 unit, uint32 flags, int32 *pBase, rtk_ipmc_addr_t *pAddr)
{
    rtdrv_ipmcCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAddr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ipmcCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.flags, &flags, sizeof(rtk_ipmc_flag_t));
    osal_memcpy(&cfg.base, pBase, sizeof(int32));
    GETSOCKOPT(RTDRV_IPMC_NEXTVALID_ADDR_GET, &cfg, rtdrv_ipmcCfg_t, 1);
    osal_memcpy(pBase, &cfg.base, sizeof(int32));
    osal_memcpy(pAddr, &cfg.ipmcEntry, sizeof(rtk_ipmc_addr_t));

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_ipmc_statMont_create
 * Description:
 *      Create a statistic monitor session
 * Input:
 *      unit  - unit id
 *      pStatMont - statistic monitor information
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
 */
int32
rtrpc_ipmc_statMont_create(uint32 unit, rtk_ipmc_statMont_t *pStatMont)
{
    rtdrv_ipmcCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatMont), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ipmcCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.statMont, pStatMont, sizeof(rtk_ipmc_statMont_t));
    SETSOCKOPT(RTDRV_IPMC_STAT_MONT_CREATE, &cfg, rtdrv_ipmcCfg_t, 1);

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_ipmc_statMont_destroy
 * Description:
 *      Destroy a statistic monitor session
 * Input:
 *      unit  - unit id
 *      pStatMont - statistic monitor information
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
rtrpc_ipmc_statMont_destroy(uint32 unit, rtk_ipmc_statMont_t *pStatMont)
{
    rtdrv_ipmcCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatMont), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ipmcCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.statMont, pStatMont, sizeof(rtk_ipmc_statMont_t));
    SETSOCKOPT(RTDRV_IPMC_STAT_MONT_DESTROY, &cfg, rtdrv_ipmcCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_ipmc_statCntr_reset
 * Description:
 *      Reset (clear) counter(s)
 * Input:
 *      unit  - unit id
 *      pKey  - key for search the statistic session
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
rtrpc_ipmc_statCntr_reset(uint32 unit, rtk_ipmc_statKey_t *pKey)
{
    rtdrv_ipmcCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pKey), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ipmcCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.statKey, pKey, sizeof(rtk_ipmc_statKey_t));
    SETSOCKOPT(RTDRV_IPMC_STAT_RESET, &cfg, rtdrv_ipmcCfg_t, 1);

    return RT_ERR_OK;
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 */
int32
rtrpc_ipmc_statCntr_get(uint32 unit, rtk_ipmc_statKey_t *pKey, rtk_ipmc_statCntr_t *pCntr)
{
    rtdrv_ipmcCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pKey), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ipmcCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.statKey, pKey, sizeof(rtk_ipmc_statKey_t));
    GETSOCKOPT(RTDRV_IPMC_STAT_GET, &cfg, rtdrv_ipmcCfg_t, 1);

    osal_memcpy(pCntr, &cfg.statCnt, sizeof(rtk_ipmc_statCntr_t));

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_ipmc_globalCtrl_set
 * Description:
 *      Set status of specified L3 multicast control
 * Input:
 *      unit  - unit id
 *      control_type - L3 multicast control type
 *      arg - control stauts
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
rtrpc_ipmc_globalCtrl_set(uint32 unit, rtk_ipmc_globalCtrlType_t controlType, int32 arg)
{
    rtdrv_ipmcCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ipmcCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.globalCtrlType, &controlType, sizeof(rtk_ipmc_globalCtrlType_t));
    osal_memcpy(&cfg.arg, &arg, sizeof(int32));
    SETSOCKOPT(RTDRV_IPMC_GLOBALCTRL_SET, &cfg, rtdrv_ipmcCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_ipmc_globalCtrl_get
 * Description:
 *      Get status of specified L3 multicast control
 * Input:
 *      unit  - unit id
 *      control_type - L3 multicast control type
 * Output:
 *      pArg - control stauts
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
rtrpc_ipmc_globalCtrl_get(uint32 unit, rtk_ipmc_globalCtrlType_t controlType, int32 *pArg)
{
    rtdrv_ipmcCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pArg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ipmcCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.globalCtrlType, &controlType, sizeof(rtk_ipmc_globalCtrlType_t));
    GETSOCKOPT(RTDRV_IPMC_GLOBALCTRL_GET, &cfg, rtdrv_ipmcCfg_t, 1);
    osal_memcpy(pArg, &cfg.arg, sizeof(int32));

    return RT_ERR_OK;
}


int32
rtrpc_ipmcMapper_init(dal_mapper_t *pMapper)
{
    pMapper->ipmc_addr_add = rtrpc_ipmc_addr_add;
    pMapper->ipmc_addr_find = rtrpc_ipmc_addr_find;
    pMapper->ipmc_addr_del = rtrpc_ipmc_addr_del;
    pMapper->ipmc_addr_delAll = rtrpc_ipmc_addr_delAll;
    pMapper->ipmc_nextValidAddr_get = rtrpc_ipmc_nextValidAddr_get;
    pMapper->ipmc_statMont_create = rtrpc_ipmc_statMont_create;
    pMapper->ipmc_statMont_destroy = rtrpc_ipmc_statMont_destroy;
    pMapper->ipmc_statCntr_reset = rtrpc_ipmc_statCntr_reset;
    pMapper->ipmc_statCntr_get = rtrpc_ipmc_statCntr_get;
    pMapper->ipmc_globalCtrl_set = rtrpc_ipmc_globalCtrl_set;
    pMapper->ipmc_globalCtrl_get = rtrpc_ipmc_globalCtrl_get;
    return RT_ERR_OK;
}
