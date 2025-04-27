/*
 * Copyright(c) Realtek Semiconductor Corporation, 2015
 * All rights reserved.
 *
 * $Revision: 83891 $
 * $Date: 2017-11-30 17:34:34 +0800 (Thu, 30 Nov 2017) $
 *
 * Purpose :
 *
 * Feature :
 *
 */

/*
 * Include Files
 */

#include <rtk/tunnel.h>
#include <dal/rtrpc/rtrpc_tunnel.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

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
 *      rtk_tunnel_init
 * Description:
 *      Initialize Tunnel module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize Tunnel module before calling any tunneling APIs.
 */
int32
rtrpc_tunnel_init(uint32 unit)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    SETSOCKOPT(RTDRV_TUNNEL_INIT, &cfg, rtdrv_tunnelCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_tunnel_init */

/* Module Name    : Tunneling                */
/* Sub-module Name: Tunneling error handling */

/* Function Name:
 *      rtk_tunnel_info_t_init
 * Description:
 *      Initialize a rtk_tunnel_info_t_init structure
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to tunnel information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
rtrpc_tunnel_info_t_init(rtk_tunnel_info_t *pInfo)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pInfo), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    osal_memcpy(&cfg.info, pInfo, sizeof(rtk_tunnel_info_t));
    SETSOCKOPT(RTDRV_TUNNEL_INFO_T_INIT, &cfg, rtdrv_tunnelCfg_t, 1);
    osal_memcpy(pInfo, &cfg.info, sizeof(rtk_tunnel_info_t));

    return RT_ERR_OK;
}   /* end of rtk_tunnel_info_t_init */

/* Function Name:
 *      rtk_tunnel_info_get
 * Description:
 *      Get tunnel-related information
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to tunnel information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
rtrpc_tunnel_info_get(uint32 unit, rtk_tunnel_info_t *pInfo)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pInfo), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_TUNNEL_INFO_GET, &cfg, rtdrv_tunnelCfg_t, 1);
    osal_memcpy(pInfo, &cfg.info, sizeof(rtk_tunnel_info_t));

    return RT_ERR_OK;
}   /* end of rtk_tunnel_info_get */

/* Function Name:
 *      rtk_tunnel_info_t_init
 * Description:
 *      Initialize a rtk_tunnel_info_t_init structure
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to tunnel information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
rtrpc_tunnel_intf_t_init(rtk_tunnel_intf_t *pIntf)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 unit = RTRPC_UNIT_NOT_USED;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    osal_memcpy(&cfg.intf, pIntf, sizeof(rtk_tunnel_intf_t));
    SETSOCKOPT(RTDRV_TUNNEL_INTF_T_INIT, &cfg, rtdrv_tunnelCfg_t, 1);
    osal_memcpy(pIntf, &cfg.intf, sizeof(rtk_tunnel_intf_t));

    return RT_ERR_OK;
}   /* end of rtk_tunnel_intf_t_init */

/* Function Name:
 *      rtk_tunnel_intf_create
 * Description:
 *      Create a new tunnel interface
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to tunnel interface containing the basic inputs
 * Output:
 *      pIntf - pointer to tunnel interface (including all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - the module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be a null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          type and corresponding fields about that tunnel.
 */
int32
rtrpc_tunnel_intf_create(uint32 unit, rtk_tunnel_intf_t *pIntf)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.intf, pIntf, sizeof(rtk_tunnel_intf_t));
    SETSOCKOPT(RTDRV_TUNNEL_INTF_CREATE, &cfg, rtdrv_tunnelCfg_t, 1);
    osal_memcpy(pIntf, &cfg.intf, sizeof(rtk_tunnel_intf_t));

    return RT_ERR_OK;
}   /* end of rtk_tunnel_intf_create */

/* Function Name:
 *      rtk_tunnel_intf_destroy
 * Description:
 *      Destroy a tunnel interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 * Note:
 */
int32
rtrpc_tunnel_intf_destroy(uint32 unit, rtk_intf_id_t intfId)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.intfId, &intfId, sizeof(rtk_intf_id_t));
    SETSOCKOPT(RTDRV_TUNNEL_INTF_DESTROY, &cfg, rtdrv_tunnelCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_tunnel_intf_destroy */

/* Function Name:
 *      rtk_tunnel_intf_destroyAll
 * Description:
 *      Destroy all tunnel interfaces
 * Input:
 *      unit  - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
rtrpc_tunnel_intf_destroyAll(uint32 unit)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    SETSOCKOPT(RTDRV_TUNNEL_INTF_DESTROYALL, &cfg, rtdrv_tunnelCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_tunnel_intf_destroyAll */

/* Function Name:
 *      rtk_tunnel_intf_get
 * Description:
 *      Get a tunnel interface by interface ID.
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to tunnel interface (interface id)
 * Output:
 *      pIntf - pointer to tunnel interface (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
int32
rtrpc_tunnel_intf_get(uint32 unit, rtk_tunnel_intf_t *pIntf)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.intf, pIntf, sizeof(rtk_tunnel_intf_t));
    GETSOCKOPT(RTDRV_TUNNEL_INTF_GET, &cfg, rtdrv_tunnelCfg_t, 1);
    osal_memcpy(pIntf, &cfg.intf, sizeof(rtk_tunnel_intf_t));

    return RT_ERR_OK;
}   /* end of rtk_tunnel_intf_get */

/* Function Name:
 *      rtk_tunnel_intf_set
 * Description:
 *      Set an tunnel interface by interface ID.
 * Input:
 *      unit  - unit id
 *      type  - search key type
 *      pIntf - pointer to tunnel interface (interface id)
 * Output:
 *      pIntf - pointer to tunnel interface (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - the module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be a null pointer
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 * Note:
 *      None
 */
int32
rtrpc_tunnel_intf_set(uint32 unit, rtk_tunnel_intf_t *pIntf)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntf), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.intf, pIntf, sizeof(rtk_tunnel_intf_t));
    SETSOCKOPT(RTDRV_TUNNEL_INTF_SET, &cfg, rtdrv_tunnelCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_tunnel_intf_set */

int32
rtrpc_tunnel_intfPathId_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_pathId_t *pPathId)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0x00, sizeof(rtdrv_tunnelCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pPathId), RT_ERR_NULL_POINTER);

    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.intfId, &intfId, sizeof(rtk_intf_id_t));
    GETSOCKOPT(RTDRV_TUNNEL_INTFPATHID_GET, &cfg, rtdrv_tunnelCfg_t, 1);
    memcpy(pPathId, &cfg.pathId, sizeof(rtk_l3_pathId_t));

    return RT_ERR_OK;
}   /* end of rtrpc_tunnel_intfPathId_get */

int32
rtrpc_tunnel_intfPathId_set(uint32 unit, rtk_intf_id_t intfId, rtk_l3_pathId_t pathId)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0x00, sizeof(rtdrv_tunnelCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.intfId, &intfId, sizeof(rtk_intf_id_t));
    memcpy(&cfg.pathId, &pathId, sizeof(rtk_l3_pathId_t));
    SETSOCKOPT(RTDRV_TUNNEL_INTFPATHID_SET, &cfg, rtdrv_tunnelCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_tunnel_intfPathId_set */

int32
rtrpc_tunnel_intfPath_get(uint32 unit, rtk_intf_id_t intfId, uint32 *pNhDmacIdx, uint32 *pL3EgrIntfIdx)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pNhDmacIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL3EgrIntfIdx), RT_ERR_NULL_POINTER);

    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.intfId, &intfId, sizeof(rtk_intf_id_t));
    GETSOCKOPT(RTDRV_TUNNEL_INTFPATH_GET, &cfg, rtdrv_tunnelCfg_t, 1);
    memcpy(pNhDmacIdx, &cfg.nhDmacIdx, sizeof(uint32));
    memcpy(pL3EgrIntfIdx, &cfg.l3EgrIntfIdx, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtrpc_tunnel_intfPath_get */

int32
rtrpc_tunnel_intfPath_set(uint32 unit, rtk_intf_id_t intfId, uint32 nhDmacIdx, uint32 l3EgrIntfIdx)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.intfId, &intfId, sizeof(rtk_intf_id_t));
    memcpy(&cfg.nhDmacIdx, &nhDmacIdx, sizeof(uint32));
    memcpy(&cfg.l3EgrIntfIdx, &l3EgrIntfIdx, sizeof(uint32));
    SETSOCKOPT(RTDRV_TUNNEL_INTFPATH_SET, &cfg, rtdrv_tunnelCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_tunnel_intfPath_set */

int32
rtrpc_tunnel_intfStats_get(uint32 unit, rtk_intf_id_t intfId, rtk_tunnel_intf_stats_t *pStats)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.intfId, &intfId, sizeof(rtk_intf_id_t));
    GETSOCKOPT(RTDRV_TUNNEL_INTFSTATS_GET, &cfg, rtdrv_tunnelCfg_t, 1);
    osal_memcpy(pStats, &cfg.stats, sizeof(rtk_tunnel_intf_stats_t));

    return RT_ERR_OK;
}   /* end of rtk_tunnel_intfStats_get */

int32
rtrpc_tunnel_intfStats_reset(uint32 unit, rtk_intf_id_t intfId)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.intfId, &intfId, sizeof(rtk_intf_id_t));
    SETSOCKOPT(RTDRV_TUNNEL_INTFSTATS_RESET, &cfg, rtdrv_tunnelCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_tunnel_intfStats_reset */

/* Function Name:
 *      rtk_tunnel_qosProfile_get
 * Description:
 *      Get the QoS profile with the specified index.
 * Input:
 *      unit     - unit id
 *      idx      - index of QoS profile
 * Output:
 *      pProfile - pointer to the QoS prifle
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
rtrpc_tunnel_qosProfile_get(uint32 unit, uint32 idx, rtk_tunnel_qosProfile_t *pProfile)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pProfile), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.idx, &idx, sizeof(uint32));
    GETSOCKOPT(RTDRV_TUNNEL_QOSPROFILE_GET, &cfg, rtdrv_tunnelCfg_t, 1);
    osal_memcpy(pProfile, &cfg.profile, sizeof(rtk_tunnel_qosProfile_t));

    return RT_ERR_OK;
}   /* end of rtk_tunnel_qosProfile_get */

/* Function Name:
 *      rtk_tunnel_qosProfile_set
 * Description:
 *      Set the QoS profile with the specified index.
 * Input:
 *      unit     - unit id
 *      idx      - index of QoS profile
 *      pProfile - pointer to the QoS prifle
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 */
int32
rtrpc_tunnel_qosProfile_set(uint32 unit, uint32 idx, rtk_tunnel_qosProfile_t profile)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.idx, &idx, sizeof(uint32));
    osal_memcpy(&cfg.profile, &profile, sizeof(rtk_tunnel_qosProfile_t));
    SETSOCKOPT(RTDRV_TUNNEL_QOSPROFILE_SET, &cfg, rtdrv_tunnelCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_tunnel_qosProfile_set */


/* Function Name:
 *      rtk_tunnel_globalCtrl_get
 * Description:
 *      Get the global configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 * Output:
 *      pArg - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
rtrpc_tunnel_globalCtrl_get(uint32 unit, rtk_tunnel_globalCtrlType_t type, int32 *pArg)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pArg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.type, &type, sizeof(rtk_tunnel_globalCtrlType_t));
    GETSOCKOPT(RTDRV_TUNNEL_GLOBALCTRL_GET, &cfg, rtdrv_tunnelCfg_t, 1);
    osal_memcpy(pArg, &cfg.arg, sizeof(int32));

    return RT_ERR_OK;
}   /* end of rtk_tunnel_globalCtrl_get */

/* Function Name:
 *      rtk_tunnel_globalCtrl_set
 * Description:
 *      Set the global configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 *      arg  - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
int32
rtrpc_tunnel_globalCtrl_set(uint32 unit, rtk_tunnel_globalCtrlType_t type, int32 arg)
{
    rtdrv_tunnelCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_tunnelCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.type, &type, sizeof(rtk_tunnel_globalCtrlType_t));
    osal_memcpy(&cfg.arg, &arg, sizeof(int32));
    SETSOCKOPT(RTDRV_TUNNEL_GLOBALCTRL_SET, &cfg, rtdrv_tunnelCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_tunnel_globalCtrl_set */

int32
rtrpc_tunnelMapper_init(dal_mapper_t *pMapper)
{
    pMapper->tunnel_info_get = rtrpc_tunnel_info_get;
    pMapper->tunnel_intf_create = rtrpc_tunnel_intf_create;
    pMapper->tunnel_intf_destroy = rtrpc_tunnel_intf_destroy;
    pMapper->tunnel_intf_destroyAll = rtrpc_tunnel_intf_destroyAll;
    pMapper->tunnel_intf_get = rtrpc_tunnel_intf_get;
    pMapper->tunnel_intf_set = rtrpc_tunnel_intf_set;
    pMapper->tunnel_intfStats_get = rtrpc_tunnel_intfStats_get;
    pMapper->tunnel_intfStats_reset = rtrpc_tunnel_intfStats_reset;
    pMapper->tunnel_qosProfile_get = rtrpc_tunnel_qosProfile_get;
    pMapper->tunnel_qosProfile_set = rtrpc_tunnel_qosProfile_set;
    pMapper->tunnel_globalCtrl_get = rtrpc_tunnel_globalCtrl_get;
    pMapper->tunnel_globalCtrl_set = rtrpc_tunnel_globalCtrl_set;
    return RT_ERR_OK;
}

