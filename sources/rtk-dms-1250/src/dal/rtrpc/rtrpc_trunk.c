/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 83476 $
 * $Date: 2017-11-15 15:13:18 +0800 (Wed, 15 Nov 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) trunk
 *
 */

#include <rtk/trunk.h>
#include <dal/rtrpc/rtrpc_trunk.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>


int32
rtrpc_trunk_mode_get (uint32 unit, rtk_trunk_mode_t *pMode)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRUNK_MODE_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pMode = trunk_cfg.mode;

    return RT_ERR_OK;
}

int32
rtrpc_trunk_mode_set(uint32 unit, rtk_trunk_mode_t mode)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.mode = mode;
    SETSOCKOPT(RTDRV_TRUNK_MODE_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_trunk_port_get(uint32 unit, uint32 trk_gid, rtk_portmask_t *pTrunk_member_portmask)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    GETSOCKOPT(RTDRV_TRUNK_PORT_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    osal_memcpy(pTrunk_member_portmask, &trunk_cfg.trk_member, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}

int32 rtrpc_trunk_port_set(uint32 unit, uint32 trk_gid, rtk_portmask_t *pTrunk_member_portmask)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    osal_memcpy(&trunk_cfg.trk_member, pTrunk_member_portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_TRUNK_PORT_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_trunk_localPort_get (uint32 unit, rtk_trk_t trk_gid, rtk_portmask_t *pTrk_local_ports)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    GETSOCKOPT(RTDRV_TRUNK_LOCAL_PORT_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    osal_memcpy(pTrk_local_ports, &trunk_cfg.trk_member, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}

int32
rtrpc_trunk_localPort_set (uint32 unit, rtk_trk_t trk_gid, rtk_portmask_t *pTrk_local_ports)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    osal_memcpy(&trunk_cfg.trk_member, pTrk_local_ports, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_TRUNK_LOCAL_PORT_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_trunk_egrPort_get(uint32 unit, rtk_trk_t trk_gid, rtk_trk_egrPort_t *pTrk_egr_ports)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    GETSOCKOPT(RTDRV_TRUNK_EGR_PORT_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    osal_memcpy(pTrk_egr_ports, &trunk_cfg.trk_egr_ports, sizeof(rtk_trk_egrPort_t));

    return RT_ERR_OK;
}

int32
rtrpc_trunk_egrPort_set(uint32 unit, rtk_trk_t trk_gid, rtk_trk_egrPort_t *pTrk_egr_ports)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    osal_memcpy(&trunk_cfg.trk_egr_ports, pTrk_egr_ports, sizeof(rtk_trk_egrPort_t));

    SETSOCKOPT(RTDRV_TRUNK_EGR_PORT_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_trunk_tunnelHashSrc_get(uint32 unit, rtk_trunk_tunnelHashSrc_t *pTunnelHashSrc)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRUNK_TUNNEL_HASH_SRC_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pTunnelHashSrc = trunk_cfg.tunnelHashSrc;

    return RT_ERR_OK;
}

int32
rtrpc_trunk_tunnelHashSrc_set(uint32 unit, rtk_trunk_tunnelHashSrc_t tunnelHashSrc)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.tunnelHashSrc = tunnelHashSrc;

    SETSOCKOPT(RTDRV_TRUNK_TUNNEL_HASH_SRC_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_trunk_distributionAlgorithmBind_get(uint32 unit, uint32 trk_gid, uint32 *pAlgo_idx)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    GETSOCKOPT(RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_BIND_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pAlgo_idx = trunk_cfg.algo_id;

    return RT_ERR_OK;
}

int32 rtrpc_trunk_distributionAlgorithmBind_set(uint32 unit, uint32 trk_gid, uint32 algo_idx)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    trunk_cfg.algo_id = algo_idx;

    SETSOCKOPT(RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_BIND_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_trunk_distributionAlgorithmTypeBind_get(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_bindType_t type, uint32 *pAlgo_idx)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    trunk_cfg.bindType= type;
    GETSOCKOPT(RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_TYPE_BIND_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pAlgo_idx = trunk_cfg.algo_id;

    return RT_ERR_OK;
}

int32
rtrpc_trunk_distributionAlgorithmTypeBind_set(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_bindType_t type, uint32 algo_idx)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    trunk_cfg.algo_id = algo_idx;
    trunk_cfg.bindType= type;

    SETSOCKOPT(RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_TYPE_BIND_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_trunk_distributionAlgorithmParam_get(uint32 unit, uint32 algo_idx, uint32 *pAlgo_bitmask)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.algo_id = algo_idx;
    GETSOCKOPT(RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_PARAM_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pAlgo_bitmask = trunk_cfg.algo_bitmask;

    return RT_ERR_OK;
}

int32 rtrpc_trunk_distributionAlgorithmParam_set(uint32 unit, uint32 algo_idx, uint32 algo_bitmask)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.algo_id = algo_idx;
    trunk_cfg.algo_bitmask = algo_bitmask;
    SETSOCKOPT(RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_PARAM_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_trunk_distributionAlgorithmTypeParam_get(uint32 unit, rtk_trunk_hashParamType_t type, uint32 algo_idx, uint32 *pAlgo_bitmask)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.algo_id = algo_idx;
    trunk_cfg.paramType = type;

    GETSOCKOPT(RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_TYPE_PARAM_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pAlgo_bitmask = trunk_cfg.algo_bitmask;

    return RT_ERR_OK;
}

int32
rtrpc_trunk_distributionAlgorithmTypeParam_set(uint32 unit, rtk_trunk_hashParamType_t type, uint32 algo_idx, uint32 algo_bitmask)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.algo_id = algo_idx;
    trunk_cfg.paramType = type;
    trunk_cfg.algo_bitmask = algo_bitmask;
    SETSOCKOPT(RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_TYPE_PARAM_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_trunk_distributionAlgorithmShift_get(uint32 unit, uint32 algo_idx, rtk_trunk_distAlgoShift_t *pShift)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.algo_id = algo_idx;
    GETSOCKOPT(RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_SHIFT_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pShift = trunk_cfg.shift;

    return RT_ERR_OK;
}

int32 rtrpc_trunk_distributionAlgorithmShift_set(uint32 unit, uint32 algo_idx, rtk_trunk_distAlgoShift_t *pShift)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.algo_id = algo_idx;
    trunk_cfg.shift = *pShift;
    SETSOCKOPT(RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_SHIFT_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_trunk_distributionAlgorithmShiftGbl_get
 * Description:
 *      Get the global shift bits of distribution algorithm parameters from the specified device.
 * Input:
 *      unit     - unit id
 * Output:
 *      pShift   - pointer buffer of shift bits of the distribution algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Instead use the fixed hash algorithm provided by the device, the API can shift each hash algorithm
 *      factor to have different distribution path.
 *      (2) Valid shift range is from 0 to 5 bits.
 */
int32
rtrpc_trunk_distributionAlgorithmShiftGbl_get(uint32 unit, rtk_trunk_distAlgoShift_t *pShift)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_SHIFT_GBL_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pShift = trunk_cfg.shift;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_trunk_distributionAlgorithmShiftGbl_set
 * Description:
 *      Set the global shift bits of distribution algorithm parameters from the specified device.
 * Input:
 *      unit     - unit id
 *      pShift   - shift bits of the distribution algorithm parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_LA_ALGO_SHIFT - invalid trunk algorithm shift
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Instead use the fixed hash algorithm provided by the device, the API can shift each hash algorithm
 *      factor to have different distribution path.
 *      (2) Valid shift range is from 0 to 5 bits.
 */
int32
rtrpc_trunk_distributionAlgorithmShiftGbl_set(uint32 unit, rtk_trunk_distAlgoShift_t *pShift)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.shift = *pShift;
    SETSOCKOPT(RTDRV_TRUNK_DISTRIBUTION_ALGORITHM_SHIFT_GBL_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_trunk_trafficSeparate_get(uint32 unit, uint32 trk_gid, rtk_trunk_separateType_t *pSeparateType)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    GETSOCKOPT(RTDRV_TRUNK_TRAFFIC_SEPARATE_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pSeparateType = trunk_cfg.separate;

    return RT_ERR_OK;
}

int32 rtrpc_trunk_trafficSeparate_set(uint32 unit, uint32 trk_gid, rtk_trunk_separateType_t separateType)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    trunk_cfg.separate = separateType;
    SETSOCKOPT(RTDRV_TRUNK_TRAFFIC_SEPARATE_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_trunk_trafficSeparateEnable_get(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_separateType_t separateType, rtk_enable_t *pEnable)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    trunk_cfg.separate = separateType;
    GETSOCKOPT(RTDRV_TRUNK_TRAFFIC_SEPARATE_ENABLE_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pEnable = trunk_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_trunk_trafficSeparateEnable_set(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_separateType_t separateType, rtk_enable_t enable)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = trk_gid;
    trunk_cfg.separate = separateType;
    trunk_cfg.enable = enable;
    SETSOCKOPT(RTDRV_TRUNK_TRAFFIC_SEPARATE_ENABLE_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_trunk_trafficSeparateDivision_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRUNK_TRAFFIC_SEPARATE_DIVISION_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pEnable = trunk_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_trunk_trafficSeparateDivision_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.enable = enable;
    SETSOCKOPT(RTDRV_TRUNK_TRAFFIC_SEPARATE_DIVISION_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_trunk_stkTrkPort_get(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_portmask_t *pStkPorts)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = stk_trk_gid;
    GETSOCKOPT(RTDRV_TRUNK_STACK_TRUNK_PORT_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pStkPorts = trunk_cfg.trk_member;

    return RT_ERR_OK;
}

int32
rtrpc_trunk_stkTrkPort_set(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_portmask_t *pStkPorts)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = stk_trk_gid;
    trunk_cfg.trk_member = *pStkPorts;
    SETSOCKOPT(RTDRV_TRUNK_STACK_TRUNK_PORT_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_trunk_stkTrkHash_get(uint32 unit, rtk_trunk_stkTrkHash_t *pStkTrkHash)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRUNK_STACK_TRUNK_HASH_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pStkTrkHash = trunk_cfg.stkTrkHash;

    return RT_ERR_OK;
}

int32
rtrpc_trunk_stkTrkHash_set(uint32 unit, rtk_trunk_stkTrkHash_t stkTrkHash)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.stkTrkHash = stkTrkHash;
    SETSOCKOPT(RTDRV_TRUNK_STACK_TRUNK_HASH_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_trunk_stkDistributionAlgorithmTypeBind_get(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_trunk_bindType_t type, uint32 *pAlgo_idx)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = stk_trk_gid;
    trunk_cfg.bindType = type;
    GETSOCKOPT(RTDRV_TRUNK_STACK_DIST_ALGO_TYPE_BIND_GET, &trunk_cfg, rtdrv_trunkCfg_t, 1);
    *pAlgo_idx = trunk_cfg.algo_id;

    return RT_ERR_OK;
}

int32
rtrpc_trunk_stkDistributionAlgorithmTypeBind_set(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_trunk_bindType_t type, uint32 algo_idx)
{
    rtdrv_trunkCfg_t trunk_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&trunk_cfg, 0, sizeof(trunk_cfg));
    trunk_cfg.unit = unit;
    trunk_cfg.trk_gid = stk_trk_gid;
    trunk_cfg.bindType = type;
    trunk_cfg.algo_id = algo_idx;
    SETSOCKOPT(RTDRV_TRUNK_STACK_DIST_ALGO_TYPE_BIND_SET, &trunk_cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}

/*
 * Function Declaration
 *      rtk_trunk_localFirst_get
 * Description:
 *      Get the local-first load-balacing enable status from the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      pLocalFirst - pointer to local-first load-balancing enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtrpc_trunk_localFirst_get(uint32 unit, rtk_enable_t *pLocalFirst)
{
    rtdrv_trunkCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRUNK_LOCALFIRST_GET, &cfg, rtdrv_trunkCfg_t, 1);
    *pLocalFirst = cfg.localFirst;

    return RT_ERR_OK;
}   /* end of rtk_trunk_localFirst_get */

/* Function Name:
 *      rtk_trunk_localFirst_set
 * Description:
 *      Set the local-first load-balacing enable status to the specified device.
 * Input:
 *      unit        - unit id
 *      localFirst - local-first load-balancing enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_INPUT       - Invalid input parameter
 * Note:
 *      Local first load balancing only works in stacking mode
 */
int32
rtrpc_trunk_localFirst_set(uint32 unit, rtk_enable_t localFirst)
{
    rtdrv_trunkCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    cfg.localFirst = localFirst;
    SETSOCKOPT(RTDRV_TRUNK_LOCALFIRST_SET, &cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trunk_localFirst_set */

/* Function Name:
 *      rtk_trunk_localFirstFailOver_get
 * Description:
 *      Get the local-first load balacing congest and link-fail avoidance enable status from the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      pCongstAvoid - pointer to congest avoidance enable status
 *      pLinkFailAvoid - pointer to link fail avoidance enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      the failover funtion only works when local-first load balancing is enabled
 */
int32
rtrpc_trunk_localFirstFailOver_get(uint32 unit, rtk_enable_t *pCongstAvoid, rtk_enable_t *pLinkFailAvoid)
{
    rtdrv_trunkCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit = unit;
    GETSOCKOPT(RTDRV_TRUNK_LOCALFIRSTFAILOVER_GET, &cfg, rtdrv_trunkCfg_t, 1);
    *pCongstAvoid = cfg.congstAvoid;
    *pLinkFailAvoid = cfg.linkFailAvoid;

    return RT_ERR_OK;
}   /* end of rtk_trunk_localFirstFailOver_get */

/* Function Name:
 *      rtk_trunk_localFirstFailOver_set
 * Description:
 *      Set the local-first load balacing congest and link-fail avoidance enable status to the specified device.
 * Input:
 *      unit          - unit id
 *      congstAvoid - congest avoidance enable status
 *      linkFailAvoid - link fail avoidance enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      the failover funtion only works when local-first load balancing is enabled
 */
int32
rtrpc_trunk_localFirstFailOver_set(uint32 unit, rtk_enable_t congstAvoid, rtk_enable_t linkFailAvoid)
{
    rtdrv_trunkCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    cfg.unit  = unit;
    cfg.congstAvoid = congstAvoid;
    cfg.linkFailAvoid = linkFailAvoid;
    SETSOCKOPT(RTDRV_TRUNK_LOCALFIRSTFAILOVER_SET, &cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trunk_localFirstFailOver_set */

/* Function Name:
 *      rtk_trunk_srcPortMap_get
 * Description:
 *      Get the info about whether DEV+Port belongs to some trunk and if yes, get its trunk ID.
 * Input:
 *      unit                   - unit id
 *      devPort             - device port
 * Output:
 *      pIsTrkMbr - pointer to get trunk or not
 *      pTrk_gid   - pointer to get trunk id.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32
rtrpc_trunk_srcPortMap_get(uint32 unit,  rtk_dev_port_t devPort, uint32 *pIsTrkMbr, rtk_trk_t *pTrk_gid)
{
    rtdrv_trunkCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIsTrkMbr), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pTrk_gid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.devPort, &devPort, sizeof(rtk_dev_port_t));
    GETSOCKOPT(RTDRV_TRUNK_SRCPORTMAP_GET, &cfg, rtdrv_trunkCfg_t, 1);
    osal_memcpy(pIsTrkMbr, &cfg.isTrkMbr, sizeof(uint32));
    osal_memcpy(pTrk_gid, &cfg.trk_gid, sizeof(rtk_trk_t));

    return RT_ERR_OK;
}   /* end of rtk_trunk_srcPortMap_get */

/* Function Name:
 *      rtk_trunk_srcPortMap_set
 * Description:
 *      Set the info about whether DEV+Port belongs to some trunk and if yes, set its trunk ID.
 * Input:
 *      unit                   - unit id
 *      devPort             - device port
 *      isTrkMbr            - trunk or not
 *      trk_gid              -  trunk id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32
rtrpc_trunk_srcPortMap_set(uint32 unit, rtk_dev_port_t devPort, uint32 isTrkMbr, rtk_trk_t trk_gid)
{
    rtdrv_trunkCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(cfg));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.devPort, &devPort, sizeof(rtk_dev_port_t));
    osal_memcpy(&cfg.isTrkMbr, &isTrkMbr, sizeof(uint32));
    osal_memcpy(&cfg.trk_gid, &trk_gid, sizeof(rtk_trk_t));
    SETSOCKOPT(RTDRV_TRUNK_SRCPORTMAP_SET, &cfg, rtdrv_trunkCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_trunk_srcPortMap_set */



int32
rtrpc_trunkMapper_init(dal_mapper_t *pMapper)
{
    pMapper->trunk_mode_get = rtrpc_trunk_mode_get ;
    pMapper->trunk_mode_set = rtrpc_trunk_mode_set;
    pMapper->trunk_port_get = rtrpc_trunk_port_get;
    pMapper->trunk_port_set = rtrpc_trunk_port_set;
    pMapper->trunk_localPort_get = rtrpc_trunk_localPort_get ;
    pMapper->trunk_localPort_set = rtrpc_trunk_localPort_set ;
    pMapper->trunk_egrPort_get = rtrpc_trunk_egrPort_get;
    pMapper->trunk_egrPort_set = rtrpc_trunk_egrPort_set;
    pMapper->trunk_tunnelHashSrc_get = rtrpc_trunk_tunnelHashSrc_get;
    pMapper->trunk_tunnelHashSrc_set = rtrpc_trunk_tunnelHashSrc_set;
    pMapper->trunk_distributionAlgorithmBind_get = rtrpc_trunk_distributionAlgorithmBind_get;
    pMapper->trunk_distributionAlgorithmBind_set = rtrpc_trunk_distributionAlgorithmBind_set;
    pMapper->trunk_distributionAlgorithmTypeBind_get = rtrpc_trunk_distributionAlgorithmTypeBind_get;
    pMapper->trunk_distributionAlgorithmTypeBind_set = rtrpc_trunk_distributionAlgorithmTypeBind_set;
    pMapper->trunk_distributionAlgorithmParam_get = rtrpc_trunk_distributionAlgorithmParam_get;
    pMapper->trunk_distributionAlgorithmParam_set = rtrpc_trunk_distributionAlgorithmParam_set;
    pMapper->trunk_distributionAlgorithmTypeParam_get = rtrpc_trunk_distributionAlgorithmTypeParam_get;
    pMapper->trunk_distributionAlgorithmTypeParam_set = rtrpc_trunk_distributionAlgorithmTypeParam_set;
    pMapper->trunk_distributionAlgorithmShift_get = rtrpc_trunk_distributionAlgorithmShift_get;
    pMapper->trunk_distributionAlgorithmShift_set = rtrpc_trunk_distributionAlgorithmShift_set;
    pMapper->trunk_distributionAlgorithmShiftGbl_get = rtrpc_trunk_distributionAlgorithmShiftGbl_get;
    pMapper->trunk_distributionAlgorithmShiftGbl_set = rtrpc_trunk_distributionAlgorithmShiftGbl_set;
    pMapper->trunk_trafficSeparate_get = rtrpc_trunk_trafficSeparate_get;
    pMapper->trunk_trafficSeparate_set = rtrpc_trunk_trafficSeparate_set;
    pMapper->trunk_trafficSeparateEnable_get = rtrpc_trunk_trafficSeparateEnable_get;
    pMapper->trunk_trafficSeparateEnable_set = rtrpc_trunk_trafficSeparateEnable_set;
    pMapper->trunk_trafficSeparateDivision_get = rtrpc_trunk_trafficSeparateDivision_get;
    pMapper->trunk_trafficSeparateDivision_set = rtrpc_trunk_trafficSeparateDivision_set;
    pMapper->trunk_stkTrkPort_get = rtrpc_trunk_stkTrkPort_get;
    pMapper->trunk_stkTrkPort_set = rtrpc_trunk_stkTrkPort_set;
    pMapper->trunk_stkTrkHash_get = rtrpc_trunk_stkTrkHash_get;
    pMapper->trunk_stkTrkHash_set = rtrpc_trunk_stkTrkHash_set;
    pMapper->trunk_stkDistributionAlgorithmTypeBind_get = rtrpc_trunk_stkDistributionAlgorithmTypeBind_get;
    pMapper->trunk_stkDistributionAlgorithmTypeBind_set = rtrpc_trunk_stkDistributionAlgorithmTypeBind_set;
    pMapper->trunk_localFirst_get = rtrpc_trunk_localFirst_get;
    pMapper->trunk_localFirst_set = rtrpc_trunk_localFirst_set;
    pMapper->trunk_localFirstFailOver_get = rtrpc_trunk_localFirstFailOver_get;
    pMapper->trunk_localFirstFailOver_set = rtrpc_trunk_localFirstFailOver_set;
    pMapper->trunk_srcPortMap_get = rtrpc_trunk_srcPortMap_get;
    pMapper->trunk_srcPortMap_set = rtrpc_trunk_srcPortMap_set;
    return RT_ERR_OK;
}
