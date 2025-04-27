/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: 83233 $
 * $Date: 2017-11-08 18:11:44 +0800 (Wed, 08 Nov 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) vlan
 *
 */

#include <dal/rtrpc/rtrpc_vlan.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 rtrpc_vlan_port_add(uint32 unit, rtk_vlan_t vid, rtk_port_t port, uint32 is_untag)
{
    rtdrv_vlan_port_t vlan_port;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_port, 0, sizeof(rtdrv_vlan_port_t));
    vlan_port.unit = unit;
    vlan_port.vid = vid;
    vlan_port.port = port;
    vlan_port.is_untag = is_untag;
    SETSOCKOPT(RTDRV_VLAN_PORT_ADD, &vlan_port, rtdrv_vlan_port_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_port_get(uint32 unit, rtk_vlan_t vid, rtk_portmask_t *pMember_portmask, rtk_portmask_t *pUntag_portmask)
{
    rtdrv_vlan_port_t vlan_port;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_port, 0, sizeof(rtdrv_vlan_port_t));
    vlan_port.unit = unit;
    vlan_port.vid = vid;
    GETSOCKOPT(RTDRV_VLAN_PORT_GET, &vlan_port, rtdrv_vlan_port_t, 1);
    osal_memcpy(pMember_portmask, &vlan_port.member, sizeof(rtk_portmask_t));
    osal_memcpy(pUntag_portmask, &vlan_port.untag, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}

int32 rtrpc_vlan_port_set(uint32 unit, rtk_vlan_t vid, rtk_portmask_t *pMember_portmask, rtk_portmask_t *pUntag_portmask)
{
    rtdrv_vlan_port_t vlan_port;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_port, 0, sizeof(rtdrv_vlan_port_t));
    vlan_port.unit = unit;
    vlan_port.vid = vid;
    osal_memcpy(&vlan_port.member, pMember_portmask, sizeof(rtk_portmask_t));
    osal_memcpy(&vlan_port.untag, pUntag_portmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_VLAN_PORT_SET, &vlan_port, rtdrv_vlan_port_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_port_del(uint32 unit, rtk_vlan_t vid, rtk_port_t port)
{
    rtdrv_vlan_port_t vlan_port;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_port, 0, sizeof(rtdrv_vlan_port_t));
    vlan_port.unit = unit;
    vlan_port.vid = vid;
    vlan_port.port = port;
    SETSOCKOPT(RTDRV_VLAN_PORT_DEL, &vlan_port, rtdrv_vlan_port_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_mcastGroup_get(uint32 unit, rtk_vlan_t vid, rtk_mcast_group_t *pGroupId)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pGroupId), RT_ERR_NULL_POINTER);

    /* function body */
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.vid, &vid, sizeof(rtk_vlan_t));
    GETSOCKOPT(RTDRV_VLAN_MCASTGROUP_GET, &cfg, rtdrv_vlanCfg_t, 1);
    memcpy(pGroupId, &cfg.groupId, sizeof(rtk_mcast_group_t));

    return RT_ERR_OK;
}   /* end of rtk_vlan_mcastGroup_get */

int32
rtrpc_vlan_mcastGroup_set(uint32 unit, rtk_vlan_t vid, rtk_mcast_group_t groupId)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.vid, &vid, sizeof(rtk_vlan_t));
    memcpy(&cfg.groupId, &groupId, sizeof(rtk_mcast_group_t));
    SETSOCKOPT(RTDRV_VLAN_MCASTGROUP_SET, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vlan_mcastGroup_set */

int32 rtrpc_vlan_portPvid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 *pPvid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    GETSOCKOPT(RTDRV_VLAN_PORT_PVID_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pPvid = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portPvid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 pvid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    vlan_cfg.data = pvid;
    SETSOCKOPT(RTDRV_VLAN_PORT_PVID_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_portPrivateVlanEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_VLAN_PORTPRIVATEVLANENABLE_GET, &cfg, rtdrv_vlanCfg_t, 1);
    memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_vlan_portPrivateVlanEnable_get */

int32
rtrpc_vlan_portPrivateVlanEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_VLAN_PORTPRIVATEVLANENABLE_SET, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vlan_portPrivateVlanEnable_set */

int32 rtrpc_vlan_portAcceptFrameType_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_acceptFrameType_t *pAccept_frame_type)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    GETSOCKOPT(RTDRV_VLAN_PORT_ACCEPT_FRAME_TYPE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pAccept_frame_type = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portAcceptFrameType_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_acceptFrameType_t accept_frame_type)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    vlan_cfg.data = accept_frame_type;
    SETSOCKOPT(RTDRV_VLAN_PORT_ACCEPT_FRAME_TYPE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrFilterEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_EGR_FILTER_ENABLE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pEnable = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrFilterEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = enable;
    SETSOCKOPT(RTDRV_VLAN_PORT_EGR_FILTER_ENABLE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_mcastLeakyEnable_get(uint32 unit, rtk_enable_t *pLeaky)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));
    unit_cfg.unit = unit;
    GETSOCKOPT(RTDRV_VLAN_EN_MCAST_LEAKY_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pLeaky = unit_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_mcastLeakyEnable_set(uint32 unit, rtk_enable_t leaky)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));
    unit_cfg.unit = unit;
    unit_cfg.data = leaky;
    SETSOCKOPT(RTDRV_VLAN_EN_MCAST_LEAKY_SET, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_vlan_svlMode_get
 * Description:
 *      Get VLAN SVL mode from the specified device.
 * Input:
 *      unit  - unit id
 * Output:
 *      pMode - pointer to SVL mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      (1) When CAPWAP is disabled, the SVL mode could be set to PER-VLAN mode.
 *          Otherwise, it will be fixed to PER-MAC-TYPE mode.
 *      (2) After the mode is set to PER-VLAN mode, use rtk_vlan_svlFid_set() to
 *          set the SVL FID for each VLAN.
 */
int32
rtrpc_vlan_svlMode_get(uint32 unit, rtk_vlan_svlMode_t *pMode)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_VLAN_SVLMODE_GET, &cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pMode, &cfg.mode, sizeof(rtk_vlan_svlMode_t));

    return RT_ERR_OK;
}   /* end of rtk_vlan_svlMode_get */

/* Function Name:
 *      rtk_vlan_svlMode_set
 * Description:
 *      Set VLAN SVL mode from the specified device.
 * Input:
 *      unit - unit id
 *      mode - SVL mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 * Note:
 *      (1) When CAPWAP is disabled, the SVL mode could be set to PER-VLAN mode.
 *          Otherwise, it will be fixed to PER-MAC-TYPE mode.
 *      (2) After the mode is set to PER-VLAN mode, use rtk_vlan_svlFid_set() to
 *          set the SVL FID for each VLAN.
 */
int32
rtrpc_vlan_svlMode_set(uint32 unit, rtk_vlan_svlMode_t mode)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.mode, &mode, sizeof(rtk_vlan_svlMode_t));
    SETSOCKOPT(RTDRV_VLAN_SVLMODE_SET, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vlan_svlMode_set */

/* Function Name:
 *      rtk_vlan_svlFid_get
 * Description:
 *      Get the SVL FID of the vlan from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - VLAN ID
 * Output:
 *      pFid - pointer to SVL FID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      (1) The SVL mode must be set to PER-VLAN mode, or the API cannot be used.
 *      (2) The valid range of FID is 0~4095
 */
int32
rtrpc_vlan_svlFid_get(uint32 unit, rtk_vlan_t vid, rtk_fid_t *pFid)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pFid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.vid, &vid, sizeof(rtk_vlan_t));
    GETSOCKOPT(RTDRV_VLAN_SVLFID_GET, &cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pFid, &cfg.fid, sizeof(rtk_fid_t));

    return RT_ERR_OK;
}   /* end of rtk_vlan_svlFid_get */

/* Function Name:
 *      rtk_vlan_svlFid_set
 * Description:
 *      Set the SVL FID of the vlan from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - VLAN ID
 *      fid  - SVL FID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 *      (1) The SVL mode must be set to PER-VLAN mode, or the API cannot be used.
 *      (2) The valid range of FID is 0~4095
 */
int32
rtrpc_vlan_svlFid_set(uint32 unit, rtk_vlan_t vid, rtk_fid_t fid)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.vid, &vid, sizeof(rtk_vlan_t));
    osal_memcpy(&cfg.fid, &fid, sizeof(rtk_fid_t));
    SETSOCKOPT(RTDRV_VLAN_SVLFID_SET, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vlan_svlFid_set */

int32 rtrpc_vlan_stg_get(uint32 unit, rtk_vlan_t vid, rtk_stg_t *pStg)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    GETSOCKOPT(RTDRV_VLAN_STG_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pStg = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_stg_set(uint32 unit, rtk_vlan_t vid, rtk_stg_t stg)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    vlan_cfg.data = stg;
    SETSOCKOPT(RTDRV_VLAN_STG_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_l2LookupSvlFid_get(uint32 unit, rtk_vlan_l2mactype_t type, rtk_fid_t *pFid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.macType = type;
    GETSOCKOPT(RTDRV_VLAN_L2_LOOKUP_SVL_FID_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pFid = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_l2LookupSvlFid_set(uint32 unit, rtk_vlan_l2mactype_t type, rtk_fid_t fid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.macType = type;
    vlan_cfg.data = fid;
    SETSOCKOPT(RTDRV_VLAN_L2_LOOKUP_SVL_FID_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_create(uint32 unit, rtk_vlan_t vid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    SETSOCKOPT(RTDRV_VLAN_CREATE, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_destroy(uint32 unit, rtk_vlan_t vid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    SETSOCKOPT(RTDRV_VLAN_DESTROY, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_destroyAll(uint32 unit, uint32 restore_default_vlan)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));
    unit_cfg.unit = unit;
    unit_cfg.data = restore_default_vlan;
    SETSOCKOPT(RTDRV_VLAN_DESTROY_ALL, &unit_cfg, rtdrv_unitCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_protoGroup_get(
    uint32                  unit,
    uint32                  protoGroup_idx,
    rtk_vlan_protoGroup_t   *pProtoGroup)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = protoGroup_idx;
    GETSOCKOPT(RTDRV_VLAN_PROTO_GROUP_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pProtoGroup, &(vlan_cfg.protoGroup), sizeof(rtk_vlan_protoGroup_t));

    return RT_ERR_OK;
}

int32 rtrpc_vlan_protoGroup_set(
    uint32                  unit,
    uint32                  protoGroup_idx,
    rtk_vlan_protoGroup_t   *pProtoGroup)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = protoGroup_idx;
    osal_memcpy(&(vlan_cfg.protoGroup), pProtoGroup, sizeof(rtk_vlan_protoGroup_t));
    SETSOCKOPT(RTDRV_VLAN_PROTO_GROUP_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portProtoVlan_get(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  protoGroup_idx,
    rtk_vlan_protoVlanCfg_t *pVlan_cfg)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.idx = protoGroup_idx;
    GETSOCKOPT(RTDRV_VLAN_PORT_PROTO_VLAN_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pVlan_cfg, &(vlan_cfg.protoVlanCfg), sizeof(rtk_vlan_protoVlanCfg_t));

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portProtoVlan_set(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  protoGroup_idx,
    rtk_vlan_protoVlanCfg_t *pVlan_cfg)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.idx = protoGroup_idx;
    osal_memcpy(&(vlan_cfg.protoVlanCfg), pVlan_cfg, sizeof(rtk_vlan_protoVlanCfg_t));
    SETSOCKOPT(RTDRV_VLAN_PORT_PROTO_VLAN_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type,uint32 *pTpid_idx_mask)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    GETSOCKOPT(RTDRV_VLAN_PORT_IGR_TPID_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pTpid_idx_mask = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type,uint32 tpid_idx_mask)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    vlan_cfg.data = tpid_idx_mask;
    SETSOCKOPT(RTDRV_VLAN_PORT_IGR_TPID_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 *pTpid_idx)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    GETSOCKOPT(RTDRV_VLAN_PORT_EGR_TPID_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pTpid_idx = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    vlan_cfg.data = tpid_idx;
    SETSOCKOPT(RTDRV_VLAN_PORT_EGR_TPID_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrTpidSrc_get(uint32 unit, rtk_port_t port,rtk_vlanType_t type, rtk_vlan_egrTpidSrc_t *pTpid_src)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    GETSOCKOPT(RTDRV_VLAN_PORT_EGR_TPID_SRC_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pTpid_src = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrTpidSrc_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_egrTpidSrc_t tpid_src)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    vlan_cfg.data = tpid_src;
    SETSOCKOPT(RTDRV_VLAN_PORT_EGR_TPID_SRC_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrExtraTagEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_IGR_EXTRA_TAG_ENABLE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pEnable = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrExtraTagEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = enable;
    SETSOCKOPT(RTDRV_VLAN_PORT_IGR_EXTRA_TAG_ENABLE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrVlanTransparentEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t  type,
    rtk_enable_t    *pEnable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    GETSOCKOPT(RTDRV_VLAN_PORT_IGR_VLAN_TRANSPARENT_ENABLE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pEnable = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrVlanTransparentEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t  type,
    rtk_enable_t    enable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    vlan_cfg.data = enable;
    SETSOCKOPT(RTDRV_VLAN_PORT_IGR_VLAN_TRANSPARENT_ENABLE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrVlanTransparentEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t  type,
    rtk_enable_t    *pEnable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    GETSOCKOPT(RTDRV_VLAN_PORT_EGR_VLAN_TRANSPARENT_ENABLE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pEnable = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrVlanTransparentEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t  type,
    rtk_enable_t    enable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    vlan_cfg.data = enable;
    SETSOCKOPT(RTDRV_VLAN_PORT_EGR_VLAN_TRANSPARENT_ENABLE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_l2LookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2mactype_t macType, rtk_vlan_l2LookupMode_t *pMode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    vlan_cfg.macType = macType;
    GETSOCKOPT(RTDRV_VLAN_LUTMODE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pMode = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_l2LookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2mactype_t macType, rtk_vlan_l2LookupMode_t mode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    vlan_cfg.macType = macType;
    vlan_cfg.data = mode;
    SETSOCKOPT(RTDRV_VLAN_LUTMODE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_groupMask_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_groupMask_t *pGroupmask)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    GETSOCKOPT(RTDRV_VLAN_GROUPMASK_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pGroupmask = vlan_cfg.groupMask;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_groupMask_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_groupMask_t *pGroupmask)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    vlan_cfg.groupMask = *pGroupmask;
    SETSOCKOPT(RTDRV_VLAN_GROUPMASK_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}
int32 rtrpc_vlan_profileIdx_get(uint32 unit, rtk_vlan_t vid, uint32 *pIdx)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    GETSOCKOPT(RTDRV_VLAN_PROFILE_IDX_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pIdx = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_profileIdx_set(uint32 unit, rtk_vlan_t vid, uint32 idx)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    vlan_cfg.data = idx;
    SETSOCKOPT(RTDRV_VLAN_PROFILE_IDX_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_profile_get(uint32 unit, uint32 idx, rtk_vlan_profile_t *pProfile)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.data = idx;
    GETSOCKOPT(RTDRV_VLAN_PROFILE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pProfile = vlan_cfg.profile;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portFwdVlan_get(uint32 unit,rtk_port_t port,rtk_vlan_pktTagMode_t tagMode,rtk_vlanType_t *pVlanType)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.tagMode = tagMode;
    GETSOCKOPT(RTDRV_VLAN_PORT_FORWARD_VLAN_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pVlanType = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portFwdVlan_set(uint32 unit,rtk_port_t port,rtk_vlan_pktTagMode_t tagMode,rtk_vlanType_t vlanType)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.tagMode = tagMode;
    vlan_cfg.data = vlanType;
    SETSOCKOPT(RTDRV_VLAN_PORT_FORWARD_VLAN_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}
int32 rtrpc_vlan_profile_set(uint32 unit, uint32 idx, rtk_vlan_profile_t *pProfile)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.data = idx;
    vlan_cfg.profile = *pProfile;
    SETSOCKOPT(RTDRV_VLAN_PROFILE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrFilter_get(uint32 unit, rtk_port_t port, rtk_vlan_ifilter_t *pIgr_filter)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_IGR_FILTER_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pIgr_filter = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrFilter_set(uint32 unit, rtk_port_t port, rtk_vlan_ifilter_t igr_filter)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = igr_filter;
    SETSOCKOPT(RTDRV_VLAN_PORT_IGR_FILTER_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portPvidMode_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_pbVlan_mode_t *pMode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    GETSOCKOPT(RTDRV_VLAN_PORT_PVID_MODE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pMode = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_pbVlan_mode_t mode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    vlan_cfg.data = mode;
    SETSOCKOPT(RTDRV_VLAN_PORT_PVID_MODE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_macBasedVlan_get(uint32 unit, uint32 index, uint32 *valid,
        rtk_mac_t *smac, rtk_vlan_t *vid, rtk_pri_t *priority)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    GETSOCKOPT(RTDRV_VLAN_MAC_BASED_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *valid = vlan_cfg.data;
    *smac = vlan_cfg.mac;
    *vid = vlan_cfg.vid;
    *priority = vlan_cfg.data1;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_macBasedVlan_set(uint32 unit, uint32 index, uint32 valid,
        rtk_mac_t *smac, rtk_vlan_t vid, rtk_pri_t priority)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    vlan_cfg.data = valid;
    vlan_cfg.mac = *smac;
    vlan_cfg.vid = vid;
    vlan_cfg.data1 = priority;
    SETSOCKOPT(RTDRV_VLAN_MAC_BASED_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_macBasedVlanWithMsk_get(uint32 unit, uint32 index, uint32 *valid,
        rtk_mac_t *smac, rtk_mac_t *smsk, rtk_vlan_t *vid, rtk_pri_t *priority)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    GETSOCKOPT(RTDRV_VLAN_MAC_BASED_WITH_MSK_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *valid = vlan_cfg.data;
    *smac = vlan_cfg.mac;
    *smsk = vlan_cfg.msk;
    *vid = vlan_cfg.vid;
    *priority = vlan_cfg.data1;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_macBasedVlanWithMsk_set(uint32 unit, uint32 index, uint32 valid,
        rtk_mac_t *smac, rtk_mac_t *smsk, rtk_vlan_t vid, rtk_pri_t priority)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    vlan_cfg.data = valid;
    vlan_cfg.mac = *smac;
    vlan_cfg.msk = *smsk;
    vlan_cfg.vid = vid;
    vlan_cfg.data1 = priority;
    SETSOCKOPT(RTDRV_VLAN_MAC_BASED_WITH_MSK_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_macBasedVlanWithPort_get(uint32 unit, uint32 index, uint32 *valid,
        rtk_mac_t *smac, rtk_mac_t *smsk, rtk_port_t *port, rtk_port_t *pmsk, rtk_vlan_t *vid, rtk_pri_t *priority)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    GETSOCKOPT(RTDRV_VLAN_MAC_BASED_WITH_PORT_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *valid = vlan_cfg.data;
    *smac = vlan_cfg.mac;
    *smsk = vlan_cfg.msk;
    *port = vlan_cfg.port;
    *pmsk = vlan_cfg.port_msk;
    *vid = vlan_cfg.vid;
    *priority = vlan_cfg.data1;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_macBasedVlanWithPort_set(uint32 unit, uint32 index, uint32 valid,
        rtk_mac_t *smac, rtk_mac_t *smsk, rtk_port_t port, rtk_port_t pmsk, rtk_vlan_t vid, rtk_pri_t priority)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    vlan_cfg.data = valid;
    vlan_cfg.mac = *smac;
    vlan_cfg.msk = *smsk;
    vlan_cfg.port = port;
    vlan_cfg.port_msk = pmsk;
    vlan_cfg.vid = vid;
    vlan_cfg.data1 = priority;
    SETSOCKOPT(RTDRV_VLAN_MAC_BASED_WITH_PORT_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portMacBasedVlanEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_MAC_BASED_ENABLE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pEnable = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portMacBasedVlanEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = enable;
    SETSOCKOPT(RTDRV_VLAN_MAC_BASED_ENABLE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_macBasedVlanEntry_get(uint32 unit,uint32 index,rtk_vlan_macVlanEntry_t *pEntry)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    GETSOCKOPT(RTDRV_VLAN_MAC_BASED_ENTRY_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pEntry = vlan_cfg.macEntry;

    return RT_ERR_OK;
}

int32
rtrpc_vlan_macBasedVlanEntry_set(uint32 unit,uint32 index,rtk_vlan_macVlanEntry_t *pEntry)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    vlan_cfg.macEntry = *pEntry;
    SETSOCKOPT(RTDRV_VLAN_MAC_BASED_ENTRY_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_macBasedVlanEntry_add(uint32 unit, rtk_vlan_macBasedEntry_t *pEntry)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.macBasedEntry, pEntry, sizeof(rtk_vlan_macBasedEntry_t));
    SETSOCKOPT(RTDRV_VLAN_MACBASEDVLANENTRY_ADD, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_vlan_macBasedVlanEntry_add */

int32
rtrpc_vlan_macBasedVlanEntry_del(uint32 unit, rtk_vlan_macBasedEntry_t *pEntry)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.macBasedEntry, pEntry, sizeof(rtk_vlan_macBasedEntry_t));
    SETSOCKOPT(RTDRV_VLAN_MACBASEDVLANENTRY_DEL, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_vlan_macBasedVlanEntry_del */

int32
rtrpc_vlan_ipSubnetBasedVlan_get(uint32 unit, uint32 index, uint32 *valid,
        ipaddr_t *sip, ipaddr_t *sip_mask, rtk_vlan_t *vid, rtk_pri_t *priority)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    GETSOCKOPT(RTDRV_VLAN_IP_SUBNET_BASED_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *valid = vlan_cfg.data;
    *sip = vlan_cfg.sip;
    *sip_mask = vlan_cfg.sip_msk;
    *vid = vlan_cfg.vid;
    *priority = vlan_cfg.data1;

    return RT_ERR_OK;
}

int32
rtrpc_vlan_ipSubnetBasedVlan_set(uint32 unit, uint32 index, uint32 valid,
        ipaddr_t sip, ipaddr_t sip_mask, rtk_vlan_t vid, rtk_pri_t priority)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    vlan_cfg.data = valid;
    vlan_cfg.sip = sip;
    vlan_cfg.sip_msk = sip_mask;
    vlan_cfg.vid = vid;
    vlan_cfg.data1 = priority;
    SETSOCKOPT(RTDRV_VLAN_IP_SUBNET_BASED_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_ipSubnetBasedVlanWithPort_get(uint32 unit, uint32 index, uint32 *valid,
        ipaddr_t *sip, ipaddr_t *sip_mask, rtk_port_t *port, rtk_port_t *port_mask, rtk_vlan_t *vid, rtk_pri_t *priority)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    GETSOCKOPT(RTDRV_VLAN_IP_SUBNET_BASED_WITH_PORT_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *valid = vlan_cfg.data;
    *sip = vlan_cfg.sip;
    *sip_mask = vlan_cfg.sip_msk;
    *port = vlan_cfg.port;
    *port_mask = vlan_cfg.port_msk;
    *vid = vlan_cfg.vid;
    *priority = vlan_cfg.data1;

    return RT_ERR_OK;
}

int32
rtrpc_vlan_ipSubnetBasedVlanWithPort_set(uint32 unit, uint32 index, uint32 valid,
        ipaddr_t sip, ipaddr_t sip_mask, rtk_port_t port, rtk_port_t port_mask, rtk_vlan_t vid, rtk_pri_t priority)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    vlan_cfg.data = valid;
    vlan_cfg.sip = sip;
    vlan_cfg.sip_msk = sip_mask;
    vlan_cfg.port = port;
    vlan_cfg.port_msk = port_mask;
    vlan_cfg.vid = vid;
    vlan_cfg.data1 = priority;
    SETSOCKOPT(RTDRV_VLAN_IP_SUBNET_BASED_WITH_PORT_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIpSubnetBasedVlanEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_IP_SUBNET_BASED_ENABLE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pEnable = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIpSubnetBasedVlanEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = enable;
    SETSOCKOPT(RTDRV_VLAN_IP_SUBNET_BASED_ENABLE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_ipSubnetBasedVlanEntry_get(uint32 unit,uint32 index,rtk_vlan_ipSubnetVlanEntry_t *pEntry)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    GETSOCKOPT(RTDRV_VLAN_IP_SUBNET_BASED_ENTRY_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pEntry = vlan_cfg.ipEntry;

    return RT_ERR_OK;
}

int32
rtrpc_vlan_ipSubnetBasedVlanEntry_set(uint32 unit,uint32 index,rtk_vlan_ipSubnetVlanEntry_t *pEntry)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    vlan_cfg.ipEntry = *pEntry;
    SETSOCKOPT(RTDRV_VLAN_IP_SUBNET_BASED_ENTRY_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_ipSubnetBasedVlanEntry_add(uint32 unit, rtk_vlan_ipSubnetVlanEntry_t *pEntry)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.ipEntry, pEntry, sizeof(rtk_vlan_ipSubnetVlanEntry_t));
    SETSOCKOPT(RTDRV_VLAN_IPSUBNETBASEDVLANENTRY_ADD, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_vlan_ipSubnetBasedVlanEntry_add */

int32
rtrpc_vlan_ipSubnetBasedVlanEntry_del(uint32 unit, rtk_vlan_ipSubnetVlanEntry_t *pEntry)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.ipEntry, pEntry, sizeof(rtk_vlan_ipSubnetVlanEntry_t));
    SETSOCKOPT(RTDRV_VLAN_IPSUBNETBASEDVLANENTRY_DEL, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_vlan_ipSubnetBasedVlanEntry_del */

int32 rtrpc_vlan_tpidEntry_get(uint32 unit, rtk_vlan_tagType_t tagType, uint32 tpid_idx, uint32 *pTpid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.tagType = tagType;
    vlan_cfg.idx = tpid_idx;
    GETSOCKOPT(RTDRV_VLAN_TPID_ENTRY_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pTpid = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_tpidEntry_set(uint32 unit, rtk_vlan_tagType_t tagType, uint32 tpid_idx, uint32 tpid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.tagType = tagType;
    vlan_cfg.idx = tpid_idx;
    vlan_cfg.data = tpid;
    SETSOCKOPT(RTDRV_VLAN_TPID_ENTRY_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrTagSts_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_tagSts_t *pSts)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    GETSOCKOPT(RTDRV_VLAN_EGR_TAG_STS_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pSts = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrTagSts_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_tagSts_t sts)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    vlan_cfg.data = sts;
    SETSOCKOPT(RTDRV_VLAN_EGR_TAG_STS_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_igrVlanCnvtBlkMode_get(uint32 unit, uint32 blk_idx, rtk_vlan_igrVlanCnvtBlk_mode_t *pMode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = blk_idx;
    GETSOCKOPT(RTDRV_VLAN_IGRVLANCNVT_BLKMODE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pMode = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_igrVlanCnvtBlkMode_set(uint32 unit, uint32 blk_idx, rtk_vlan_igrVlanCnvtBlk_mode_t mode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = blk_idx;
    vlan_cfg.data = mode;
    SETSOCKOPT(RTDRV_VLAN_IGRVLANCNVT_BLKMODE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_igrVlanCnvtEntry_get(uint32 unit, uint32 index, rtk_vlan_igrVlanCnvtEntry_t *pData)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    GETSOCKOPT(RTDRV_VLAN_IGRVLANCNVT_ENTRY_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pData = vlan_cfg.igrCnvtEntry;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_igrVlanCnvtEntry_set(uint32 unit, uint32 index, rtk_vlan_igrVlanCnvtEntry_t *pData)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    vlan_cfg.igrCnvtEntry = *pData;
    SETSOCKOPT(RTDRV_VLAN_IGRVLANCNVT_ENTRY_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_vlan_portIgrVlanCnvtEnable_get
 * Description:
 *      Get enable status of ingress VLAN conversion on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable   - pointer to enable status of ingress VLAN conversion
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtrpc_vlan_portIgrVlanCnvtEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_VLAN_PORTIGRVLANCNVTENABLE_GET, &cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_vlan_portIgrVlanCnvtEnable_get */

/* Function Name:
 *      rtk_vlan_portIgrVlanCnvtEnable_set
 * Description:
 *      Set enable status of ingress VLAN conversion on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of ingress VLAN conversion
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtrpc_vlan_portIgrVlanCnvtEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_VLAN_PORTIGRVLANCNVTENABLE_SET, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vlan_portIgrVlanCnvtEnable_set */

int32 rtrpc_vlan_egrVlanCnvtDblTagEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    vlan_cfg.unit = unit;
    GETSOCKOPT(RTDRV_VLAN_EGRVLANCNVT_DBLTAG_ENABLE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pEnable = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_egrVlanCnvtDblTagEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.data = enable;
    SETSOCKOPT(RTDRV_VLAN_EGRVLANCNVT_DBLTAG_ENABLE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_egrVlanCnvtVidSource_get(uint32 unit, rtk_l2_vlanMode_t *pSrc)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    GETSOCKOPT(RTDRV_VLAN_EGRVLANCNVT_VIDSRC_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pSrc = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_egrVlanCnvtVidSource_set(uint32 unit, rtk_l2_vlanMode_t src)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.data = src;
    SETSOCKOPT(RTDRV_VLAN_EGRVLANCNVT_VIDSRC_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_egrVlanCnvtEntry_get(uint32 unit, uint32 index, rtk_vlan_egrVlanCnvtEntry_t *pData)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    GETSOCKOPT(RTDRV_VLAN_EGRVLANCNVT_ENTRY_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pData = vlan_cfg.egrCnvtEntry;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_egrVlanCnvtEntry_set(uint32 unit, uint32 index, rtk_vlan_egrVlanCnvtEntry_t *pData)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    vlan_cfg.egrCnvtEntry = *pData;
    SETSOCKOPT(RTDRV_VLAN_EGRVLANCNVT_ENTRY_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtEnable_get
 * Description:
 *      Get enable status of egress VLAN conversion on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable   - pointer to enable status of egress VLAN conversion
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtrpc_vlan_portEgrVlanCnvtEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_VLAN_PORTEGRVLANCNVTENABLE_GET, &cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_vlan_portEgrVlanCnvtEnable_get */

/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtEnable_set
 * Description:
 *      Set enable status of egress VLAN conversion on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of egress VLAN conversion
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtrpc_vlan_portEgrVlanCnvtEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_VLAN_PORTEGRVLANCNVTENABLE_SET, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vlan_portEgrVlanCnvtEnable_set */

int32
rtrpc_vlan_aggrEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_VLAN_AGGRENABLE_GET, &cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}

int32
rtrpc_vlan_aggrEnable_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_VLAN_AGGRENABLE_SET, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portVlanAggrEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_VLANAGGR_ENABLE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pEnable = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portVlanAggrEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = enable;
    SETSOCKOPT(RTDRV_VLAN_PORT_VLANAGGR_ENABLE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_vlan_portVlanAggrPriEnable_get
 * Description:
 *      Enable N:1 VLAN-Priority aggregation support on egress port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) When the N:1 VLAN-Priority aggregation function is enabled on a egress port,
 *          the inner or outer priority (selected by rtk_vlan_portVlanAggrCtrl_set() API)
 *          of the downstream packet will be replaced by the priority which learnt in L2 table
 *          from upstream packet.
 *      (2) N:1 VLAN-Priority replacement only applies to the tagged and known unicast packet.
 */
int32
rtrpc_vlan_portVlanAggrPriEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_VLAN_PORT_VLANAGGR_PRI_ENABLE_GET, &cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_vlan_portVlanAggrPriEnable_get */

/* Function Name:
 *      rtk_vlan_portVlanAggrPriEnable_set
 * Description:
 *      Enable N:1 VLAN-Priority aggregation support on egress port.
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
 * Note:
 *      (1) When the N:1 VLAN-Priority aggregation function is enabled on a egress port,
 *          the inner or outer priority (selected by rtk_vlan_portVlanAggrCtrl_set() API)
 *          of the downstream packet will be replaced by the priority which learnt in L2 table
 *          from upstream packet.
 *      (2) N:1 VLAN-Priority replacement only applies to the tagged and known unicast packet.
 */
int32
rtrpc_vlan_portVlanAggrPriEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_VLAN_PORT_VLANAGGR_PRI_ENABLE_SET, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vlan_portVlanAggrPriEnable_set */

int32
rtrpc_vlan_leakyStpFilter_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    GETSOCKOPT(RTDRV_VLAN_LEAKYSTPFILTER_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    *pEnable = vlan_cfg.data;

    return RT_ERR_OK;
}    /* end of rtk_vlan_leakyStpFilter_get */

int32
rtrpc_vlan_leakyStpFilter_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.data = enable;
    SETSOCKOPT(RTDRV_VLAN_LEAKYSTPFILTER_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_vlan_leakyStpFilter_set */

int32
rtrpc_vlan_except_get(uint32 unit, rtk_action_t *pAction)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    GETSOCKOPT(RTDRV_VLAN_EXCEPT_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pAction = vlan_cfg.data;

    return RT_ERR_OK;
}    /* end of rtk_vlan_except_get */

int32
rtrpc_vlan_except_set(uint32 unit, rtk_action_t action)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.data = action;
    SETSOCKOPT(RTDRV_VLAN_EXCEPT_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_vlan_except_set */

int32
rtrpc_vlan_portIgrCnvtDfltAct_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORTIGRCNVTDFLTACT_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pAction = vlan_cfg.data;

    return RT_ERR_OK;
}    /* end of rtk_vlan_portIgrCnvtDfltAct_get */

int32
rtrpc_vlan_portIgrCnvtDfltAct_set(uint32 unit, rtk_port_t port, rtk_action_t action)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = action;
    SETSOCKOPT(RTDRV_VLAN_PORTIGRCNVTDFLTACT_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_vlan_portIgrCnvtDfltAct_set */


int32 rtrpc_vlan_portIgrVlanCnvtLuMisAct_get(uint32 unit, rtk_port_t port,rtk_vlanType_t type,rtk_vlan_lookupMissAct_t *pAction)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    GETSOCKOPT(RTDRV_VLAN_PORTIGRCNVTLUMISACT_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pAction = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrVlanCnvtLuMisAct_set(uint32 unit,rtk_port_t port,rtk_vlanType_t type,rtk_vlan_lookupMissAct_t action)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    vlan_cfg.data = action;
    SETSOCKOPT(RTDRV_VLAN_PORTIGRCNVTLUMISACT_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrVlanCnvtLuMisAct_get(uint32 unit, rtk_port_t port,rtk_vlanType_t type,rtk_vlan_lookupMissAct_t *pAction)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    GETSOCKOPT(RTDRV_VLAN_PORTEGRCNVTLUMISACT_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pAction = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrVlanCnvtLuMisAct_set(uint32 unit, rtk_port_t port,rtk_vlanType_t type,rtk_vlan_lookupMissAct_t action)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.type = type;
    vlan_cfg.data = action;
    SETSOCKOPT(RTDRV_VLAN_PORTEGRCNVTLUMISACT_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_portIgrTagKeepType_get(uint32 unit, rtk_port_t port, rtk_vlan_tagKeepType_t * pKeeptypeOuter, rtk_vlan_tagKeepType_t * pKeeptypeInner)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_IGRTAGKEEPTYPE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pKeeptypeOuter = vlan_cfg.data;
    *pKeeptypeInner = vlan_cfg.data1;

    return RT_ERR_OK;
} /* end of rtk_vlan_portIgrTagKeepType_get */

int32
rtrpc_vlan_portIgrTagKeepType_set(uint32 unit, rtk_port_t port, rtk_vlan_tagKeepType_t keeptypeOuter, rtk_vlan_tagKeepType_t keeptypeInner)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = keeptypeOuter;
    vlan_cfg.data1 = keeptypeInner;
    SETSOCKOPT(RTDRV_VLAN_PORT_IGRTAGKEEPTYPE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_vlan_portIgrTagKeepType_set */

int32
rtrpc_vlan_portEgrTagKeepType_get(uint32 unit, rtk_port_t port, rtk_vlan_tagKeepType_t * pKeeptypeOuter, rtk_vlan_tagKeepType_t * pKeeptypeInner)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_EGRTAGKEEPTYPE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pKeeptypeOuter = vlan_cfg.data;
    *pKeeptypeInner = vlan_cfg.data1;

    return RT_ERR_OK;
} /* end of rtk_vlan_portIgrTagKeepType_get */

int32
rtrpc_vlan_portEgrTagKeepType_set(uint32 unit, rtk_port_t port, rtk_vlan_tagKeepType_t keeptypeOuter, rtk_vlan_tagKeepType_t keeptypeInner)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = keeptypeOuter;
    vlan_cfg.data1 = keeptypeInner;
    SETSOCKOPT(RTDRV_VLAN_PORT_EGRTAGKEEPTYPE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_vlan_portIgrTagKeepType_set */

int32
rtrpc_vlan_igrVlanCnvtHitIndication_get(uint32 unit, uint32 entry_idx,uint32 flag,uint32 *pIsHit)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = entry_idx;
    vlan_cfg.flag = flag;
    GETSOCKOPT(RTDRV_VLAN_IGRVLANCNVTHITINDICATION_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pIsHit = vlan_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_vlan_egrVlanCnvtHitIndication_get(uint32 unit, uint32 entry_idx,uint32 flag,uint32 *pIsHit)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = entry_idx;
    vlan_cfg.flag = flag;
    GETSOCKOPT(RTDRV_VLAN_EGRVLANCNVTHITINDICATION_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pIsHit = vlan_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_vlan_igrVlanCnvtEntry_delAll(uint32 unit)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    SETSOCKOPT(RTDRV_VLAN_IGRVLANCNVTENTRY_DELALL, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_vlan_igrVlanCnvtEntry_delAll */

int32
rtrpc_vlan_egrVlanCnvtEntry_delAll(uint32 unit)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    SETSOCKOPT(RTDRV_VLAN_EGRVLANCNVTENTRY_DELALL, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_vlan_egrVlanCnvtEntry_delAll */

int32
rtrpc_vlan_portIgrVlanCnvtRangeCheckSet_get(uint32 unit,rtk_port_t port,uint32 *pSetIdx )
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_IGRVLANCNVTRANGECHECKSET_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pSetIdx = vlan_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_vlan_portIgrVlanCnvtRangeCheckSet_set(uint32 unit,rtk_port_t port,uint32 setIdx )
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = setIdx;
    SETSOCKOPT(RTDRV_VLAN_IGRVLANCNVTRANGECHECKSET_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_igrVlanCnvtRangeCheckEntry_get(uint32 unit, uint32 setIdx, uint32 index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t *pData)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.setIdx = setIdx;
    vlan_cfg.idx = index;
    GETSOCKOPT(RTDRV_VLAN_IGRVLANCNVTRANGECHECKENTRY_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pData, &vlan_cfg.rangeCheck, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));

    return RT_ERR_OK;
}

int32
rtrpc_vlan_igrVlanCnvtRangeCheckEntry_set(uint32 unit, uint32 setIdx, uint32 index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t *pData)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.setIdx = setIdx;
    vlan_cfg.idx = index;
    osal_memcpy(&vlan_cfg.rangeCheck, pData, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));
    SETSOCKOPT(RTDRV_VLAN_IGRVLANCNVTRANGECHECKENTRY_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_portEgrVlanCnvtRangeCheckSet_get(uint32 unit,rtk_port_t port,uint32 *pSetIdx )
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_EGRVLANCNVTRANGECHECKSET_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pSetIdx = vlan_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_vlan_portEgrVlanCnvtRangeCheckSet_set(uint32 unit,rtk_port_t port,uint32 setIdx )
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = setIdx;
    SETSOCKOPT(RTDRV_VLAN_EGRVLANCNVTRANGECHECKSET_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_egrVlanCnvtRangeCheckEntry_get(uint32 unit, uint32 setIdx, uint32 index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t *pData)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.setIdx = setIdx;
    vlan_cfg.idx = index;
    GETSOCKOPT(RTDRV_VLAN_EGRVLANCNVTRANGECHECKENTRY_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pData, &vlan_cfg.rangeCheck, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));

    return RT_ERR_OK;
}

int32
rtrpc_vlan_egrVlanCnvtRangeCheckEntry_set(uint32 unit, uint32 setIdx, uint32 index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t *pData)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.setIdx = setIdx;
    vlan_cfg.idx = index;
    osal_memcpy(&vlan_cfg.rangeCheck, pData, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));

    SETSOCKOPT(RTDRV_VLAN_EGRVLANCNVTRANGECHECKENTRY_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_portVlanAggrCtrl_get(uint32 unit, rtk_port_t port, rtk_vlan_aggrCtrl_t *pVlanAggrCtrl)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_VLANAGGRCTRL_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pVlanAggrCtrl = vlan_cfg.vlanAggrCtrl;

    return RT_ERR_OK;
}

int32
rtrpc_vlan_portVlanAggrCtrl_set(uint32 unit, rtk_port_t port, rtk_vlan_aggrCtrl_t vlanAggrCtrl)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.vlanAggrCtrl = vlanAggrCtrl;
    SETSOCKOPT(RTDRV_VLAN_PORT_VLANAGGRCTRL_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_portEgrVlanCnvtVidSource_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pSrc)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_EGRVLANCNVTVIDSOURCE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pSrc = vlan_cfg.data;

    return RT_ERR_OK;
}    /* end of rtk_vlan_portEgrVlanCnvtVidSource_get */

int32
rtrpc_vlan_portEgrVlanCnvtVidSource_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t src)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = src;
    SETSOCKOPT(RTDRV_VLAN_PORT_EGRVLANCNVTVIDSOURCE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_vlan_portEgrVlanCnvtVidSource_set */

int32
rtrpc_vlan_portEgrVlanCnvtVidTarget_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pTgt)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_EGRVLANCNVTVIDTARGET_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pTgt = vlan_cfg.data;

    return RT_ERR_OK;
}    /* end of rtk_vlan_portEgrVlanCnvtVidTarget_get */

int32
rtrpc_vlan_portEgrVlanCnvtVidTarget_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t tgt)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = tgt;
    SETSOCKOPT(RTDRV_VLAN_PORT_EGRVLANCNVTVIDTARGET_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_vlan_portEgrVlanCnvtVidTarget_set */

#ifdef CONFIG_SDK_DRIVER_RTK_LEGACY_API
int32
rtrpc_vlan_portVlanAggrVidSource_get(uint32 unit, rtk_port_t port, rtk_vlanType_t *pSrc)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_VLANAGGRVIDSOURCE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pSrc = vlan_cfg.data;

    return RT_ERR_OK;
}    /* end of rtk_vlan_portVlanAggrVidSource_get */

int32
rtrpc_vlan_portVlanAggrVidSource_set(uint32 unit, rtk_port_t port, rtk_vlanType_t src)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = src;
    SETSOCKOPT(RTDRV_VLAN_PORT_VLANAGGRVIDSOURCE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_vlan_portVlanAggrVidSource_set */

int32
rtrpc_vlan_portVlanAggrPriTagVidSource_get(uint32 unit, rtk_port_t port, rtk_vlan_priTagVidSrc_t *pSrc)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_VLANAGGRPRITAGVIDSOURCE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pSrc = vlan_cfg.data;

    return RT_ERR_OK;
}    /* end of rtk_vlan_portVlanAggrPriTagVidSource_get */

int32
rtrpc_vlan_portVlanAggrPriTagVidSource_set(uint32 unit, rtk_port_t port, rtk_vlan_priTagVidSrc_t src)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = src;
    SETSOCKOPT(RTDRV_VLAN_PORT_VLANAGGRPRITAGVIDSOURCE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_vlan_portVlanAggrPriTagVidSource_set */

int32 rtrpc_vlan_l2UcastLookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2LookupMode_t *pMode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    GETSOCKOPT(RTDRV_VLAN_UCAST_LUTMODE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pMode = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_l2UcastLookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2LookupMode_t mode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    vlan_cfg.data = mode;
    SETSOCKOPT(RTDRV_VLAN_UCAST_LUTMODE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_l2McastLookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2LookupMode_t *pMode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    GETSOCKOPT(RTDRV_VLAN_MCAST_LUTMODE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pMode = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_l2McastLookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2LookupMode_t mode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.vid = vid;
    vlan_cfg.data = mode;
    SETSOCKOPT(RTDRV_VLAN_MCAST_LUTMODE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portInnerAcceptFrameType_get(uint32 unit, rtk_port_t port, rtk_vlan_acceptFrameType_t *pAccept_frame_type)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_INNER_ACCEPT_FRAME_TYPE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pAccept_frame_type = port_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portInnerAcceptFrameType_set(uint32 unit, rtk_port_t port, rtk_vlan_acceptFrameType_t accept_frame_type)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = accept_frame_type;
    SETSOCKOPT(RTDRV_VLAN_PORT_INNER_ACCEPT_FRAME_TYPE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portOuterAcceptFrameType_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_vlan_acceptFrameType_t  *pAccept_frame_type)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_OUTER_ACCEPT_FRAME_TYPE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pAccept_frame_type = port_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portOuterAcceptFrameType_set(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_vlan_acceptFrameType_t  accept_frame_type)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = accept_frame_type;
    SETSOCKOPT(RTDRV_VLAN_PORT_OUTER_ACCEPT_FRAME_TYPE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portInnerPvidMode_get(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t *pMode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_INNER_PVID_MODE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pMode = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portInnerPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t mode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = mode;
    SETSOCKOPT(RTDRV_VLAN_PORT_INNER_PVID_MODE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portOuterPvidMode_get(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t *pMode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_OUTER_PVID_MODE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pMode = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portOuterPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t mode)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = mode;
    SETSOCKOPT(RTDRV_VLAN_PORT_OUTER_PVID_MODE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portInnerPvid_get(uint32 unit, rtk_port_t port, uint32 *pPvid)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_INNER_PVID_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pPvid = port_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portInnerPvid_set(uint32 unit, rtk_port_t port, uint32 pvid)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = pvid;
    SETSOCKOPT(RTDRV_VLAN_PORT_INNER_PVID_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portOuterPvid_get(uint32 unit, rtk_port_t port, rtk_vlan_t *pPvid)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_OUTER_PVID_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pPvid = port_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portOuterPvid_set(uint32 unit, rtk_port_t port, rtk_vlan_t pvid)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = pvid;
    SETSOCKOPT(RTDRV_VLAN_PORT_OUTER_PVID_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_innerTpidEntry_get(uint32 unit, uint32 tpid_idx, uint32 *pTpid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = tpid_idx;
    GETSOCKOPT(RTDRV_VLAN_INNER_TPID_ENTRY_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pTpid = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_innerTpidEntry_set(uint32 unit, uint32 tpid_idx, uint32 tpid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = tpid_idx;
    vlan_cfg.data = tpid;
    SETSOCKOPT(RTDRV_VLAN_INNER_TPID_ENTRY_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_outerTpidEntry_get(uint32 unit, uint32 tpid_idx, uint32 *pTpid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = tpid_idx;
    GETSOCKOPT(RTDRV_VLAN_OUTER_TPID_ENTRY_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pTpid = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_outerTpidEntry_set(uint32 unit, uint32 tpid_idx, uint32 tpid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = tpid_idx;
    vlan_cfg.data = tpid;
    SETSOCKOPT(RTDRV_VLAN_OUTER_TPID_ENTRY_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_extraTpidEntry_get(uint32 unit, uint32 tpid_idx, uint32 *pTpid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = tpid_idx;
    GETSOCKOPT(RTDRV_VLAN_EXTRA_TPID_ENTRY_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pTpid = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_extraTpidEntry_set(uint32 unit, uint32 tpid_idx, uint32 tpid)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = tpid_idx;
    vlan_cfg.data = tpid;
    SETSOCKOPT(RTDRV_VLAN_EXTRA_TPID_ENTRY_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrInnerTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx_mask)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_IGR_INNER_TPID_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pTpid_idx_mask = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrInnerTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx_mask)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = tpid_idx_mask;
    SETSOCKOPT(RTDRV_VLAN_PORT_IGR_INNER_TPID_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrOuterTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx_mask)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_IGR_OUTER_TPID_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pTpid_idx_mask = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrOuterTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx_mask)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = tpid_idx_mask;
    SETSOCKOPT(RTDRV_VLAN_PORT_IGR_OUTER_TPID_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrInnerTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_EGR_INNER_TPID_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pTpid_idx = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrInnerTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = tpid_idx;
    SETSOCKOPT(RTDRV_VLAN_PORT_EGR_INNER_TPID_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrOuterTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_EGR_OUTER_TPID_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pTpid_idx = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrOuterTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = tpid_idx;
    SETSOCKOPT(RTDRV_VLAN_PORT_EGR_OUTER_TPID_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrInnerTagSts_get(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t *pSts)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_EGR_INNER_TAG_STS_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pSts = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrInnerTagSts_set(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t sts)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = sts;
    SETSOCKOPT(RTDRV_VLAN_EGR_INNER_TAG_STS_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrOuterTagSts_get(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t *pSts)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_EGR_OUTER_TAG_STS_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pSts = vlan_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrOuterTagSts_set(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t sts)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = sts;
    SETSOCKOPT(RTDRV_VLAN_EGR_OUTER_TAG_STS_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrTagKeepEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    *pKeepOuter,
    rtk_enable_t    *pKeepInner)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_IGR_TAG_KEEP_ENABLE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pKeepOuter = vlan_cfg.data;
    *pKeepInner = vlan_cfg.data1;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portIgrTagKeepEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    keepOuter,
    rtk_enable_t    keepInner)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = keepOuter;
    vlan_cfg.data1 = keepInner;
    SETSOCKOPT(RTDRV_VLAN_PORT_IGR_TAG_KEEP_ENABLE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrTagKeepEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    *pKeepOuter,
    rtk_enable_t    *pKeepInner)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_EGR_TAG_KEEP_ENABLE_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pKeepOuter = vlan_cfg.data;
    *pKeepInner = vlan_cfg.data1;

    return RT_ERR_OK;
}

int32 rtrpc_vlan_portEgrTagKeepEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_enable_t    keepOuter,
    rtk_enable_t    keepInner)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = keepOuter;
    vlan_cfg.data1 = keepInner;
    SETSOCKOPT(RTDRV_VLAN_PORT_EGR_TAG_KEEP_ENABLE_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_vlan_portEgrVlanCnvtLookupMissAct_get(uint32 unit, rtk_port_t port, rtk_vlan_lookupMissAct_t *pAct)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    GETSOCKOPT(RTDRV_VLAN_PORT_EGRVLANCNVTLOOKUPMISSACT_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);
    *pAct = vlan_cfg.data;

    return RT_ERR_OK;
}    /* end of rtk_vlan_portEgrVlanCnvtLookupMissAct_get */

int32
rtrpc_vlan_portEgrVlanCnvtLookupMissAct_set(uint32 unit, rtk_port_t port, rtk_vlan_lookupMissAct_t act)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.port = port;
    vlan_cfg.data = act;
    SETSOCKOPT(RTDRV_VLAN_PORT_EGRVLANCNVTLOOKUPMISSACT_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_vlan_portEgrVlanCnvtLookupMissAct_set */
#endif

int32
rtrpc_vlan_egrVlanCnvtRangeCheckVid_get(uint32 unit, uint32 index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t *pData)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    GETSOCKOPT(RTDRV_VLAN_EGRVLANCNVTRANGECHECKVID_GET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    osal_memcpy(pData, &vlan_cfg.rangeCheck, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));

    return RT_ERR_OK;
}    /* end of rtk_vlan_egrVlanCnvtRangeCheckVid_get */

int32
rtrpc_vlan_egrVlanCnvtRangeCheckVid_set(uint32 unit, uint32 index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t *pData)
{
    rtdrv_vlanCfg_t vlan_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&vlan_cfg, 0, sizeof(rtdrv_vlanCfg_t));
    vlan_cfg.unit = unit;
    vlan_cfg.idx = index;
    osal_memcpy(&vlan_cfg.rangeCheck, pData, sizeof(rtk_vlan_vlanCnvtRangeCheck_vid_t));
    SETSOCKOPT(RTDRV_VLAN_EGRVLANCNVTRANGECHECKVID_SET, &vlan_cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_vlan_egrVlanCnvtRangeCheckVid_set */

int32
rtrpc_vlan_ecidPmsk_add(uint32 unit, rtk_vlan_t vid, rtk_bpe_pmskEntry_t *pEntry)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.vid, &vid, sizeof(rtk_vlan_t));
    osal_memcpy(&cfg.entry, pEntry, sizeof(rtk_bpe_pmskEntry_t));
    SETSOCKOPT(RTDRV_VLAN_ECIDPMSK_ADD, &cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pEntry, &cfg.entry, sizeof(rtk_bpe_pmskEntry_t));

    return RT_ERR_OK;
}   /* end of rtk_vlan_ecidPmsk_add */

int32
rtrpc_vlan_ecidPmsk_del(uint32 unit, rtk_vlan_t vid, rtk_bpe_pmskEntry_t *pEntry)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.vid, &vid, sizeof(rtk_vlan_t));
    osal_memcpy(&cfg.entry, pEntry, sizeof(rtk_bpe_pmskEntry_t));
    SETSOCKOPT(RTDRV_VLAN_ECIDPMSK_DEL, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vlan_ecidPmsk_del */

int32
rtrpc_vlan_ecidPmsk_get(uint32 unit, rtk_vlan_t vid, rtk_bpe_pmskEntry_t *pEntry)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.vid, &vid, sizeof(rtk_vlan_t));
    osal_memcpy(&cfg.entry, pEntry, sizeof(rtk_bpe_pmskEntry_t));
    GETSOCKOPT(RTDRV_VLAN_ECIDPMSK_GET, &cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pEntry, &cfg.entry, sizeof(rtk_bpe_pmskEntry_t));

    return RT_ERR_OK;
}   /* end of rtk_vlan_ecidPmsk_get */

int32
rtrpc_vlan_ecidPmskNextValid_get(uint32 unit, rtk_vlan_t vid, rtk_bpe_pmskEntry_t *pEntry)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.vid, &vid, sizeof(rtk_vlan_t));
    osal_memcpy(&cfg.entry, pEntry, sizeof(rtk_bpe_pmskEntry_t));
    GETSOCKOPT(RTDRV_VLAN_ECIDPMSKNEXTVALID_GET, &cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pEntry, &cfg.entry, sizeof(rtk_bpe_pmskEntry_t));

    return RT_ERR_OK;
}   /* end of rtk_vlan_ecidPmskNextValid_get */

/* Function Name:
 *      rtk_vlan_trkVlanAggrEnable_get
 * Description:
 *      Enable N:1 VLAN aggregation support on egress trunk port.
 * Input:
 *      unit - unit id
 *      tid  - trunk id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) When the N:1 VLAN aggregation function is enabled on a egress trunk port, the inner VID
 *          of the downstream packet will be replaced by the VID which learnt in L2 table from
 *          upstream packet.
 *      (2) N:1 VLAN replacement only applies to the inner tagged and known unicast packet.
 */
int32
rtrpc_vlan_trkVlanAggrEnable_get(uint32 unit, rtk_trk_t tid, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.tid, &tid, sizeof(rtk_trk_t));
    GETSOCKOPT(RTDRV_VLAN_TRKVLANAGGRENABLE_GET, &cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_vlan_trkVlanAggrEnable_get */

/* Function Name:
 *      rtk_vlan_trkVlanAggrEnable_set
 * Description:
 *      Enable N:1 VLAN aggregation support on egress trunk port.
 * Input:
 *      unit - unit id
 *      tid  - trunk id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_TRUNK_ID - invalid trunk id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) When the N:1 VLAN aggregation function is enabled on a egress trunk port, the inner VID
 *          of the downstream packet will be replaced by the VID which learnt in L2 table from
 *          upstream packet.
 *      (2) N:1 VLAN replacement only applies to the inner tagged and known unicast packet.
 */
int32
rtrpc_vlan_trkVlanAggrEnable_set(uint32 unit, rtk_trk_t tid, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.tid, &tid, sizeof(rtk_trk_t));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_VLAN_TRKVLANAGGRENABLE_SET, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vlan_trkVlanAggrEnable_set */

/* Function Name:
 *      rtk_vlan_trkVlanAggrPriEnable_get
 * Description:
 *      Enable N:1 VLAN-Priority aggregation support on egress trunk port.
 * Input:
 *      unit - unit id
 *      tid  - trunk id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) When the N:1 VLAN-Priority aggregation function is enabled on a egress trunk port,
 *          the inner or outer priority (selected by rtk_vlan_trkVlanAggrCtrl_set() API)
 *          of the downstream packet will be replaced by the priority which learnt in L2 table
 *          from upstream packet.
 *      (2) N:1 VLAN-Priority replacement only applies to the tagged and known unicast packet.
 */
int32
rtrpc_vlan_trkVlanAggrPriEnable_get(uint32 unit, rtk_trk_t tid, rtk_enable_t *pEnable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.tid, &tid, sizeof(rtk_trk_t));
    GETSOCKOPT(RTDRV_VLAN_TRKVLANAGGRPRIENABLE_GET, &cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_vlan_trkVlanAggrPriEnable_get */

/* Function Name:
 *      rtk_vlan_trkVlanAggrPriEnable_set
 * Description:
 *      Enable N:1 VLAN-Priority aggregation support on egress trunk port.
 * Input:
 *      unit   - unit id
 *      tid    - trunk id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_TRUNK_ID - invalid trunk id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) When the N:1 VLAN-Priority aggregation function is enabled on a egress trunk port,
 *          the inner or outer priority (selected by rtk_vlan_trkVlanAggrCtrl_set() API)
 *          of the downstream packet will be replaced by the priority which learnt in L2 table
 *          from upstream packet.
 *      (2) N:1 VLAN-Priority replacement only applies to the tagged and known unicast packet.
 */
int32
rtrpc_vlan_trkVlanAggrPriEnable_set(uint32 unit, rtk_trk_t tid, rtk_enable_t enable)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.tid, &tid, sizeof(rtk_trk_t));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_VLAN_TRKVLANAGGRPRIENABLE_SET, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vlan_trkVlanAggrPriEnable_set */



/* Function Name:
 *      rtk_vlan_trkVlanAggrCtrl_get
 * Description:
 *      Get trunk vlan-aggragation configuration
 * Input:
 *      unit - unit id
 *      tid  - trunk id
 * Output:
 *      pVlanAggrCtrl - pointer to vlan-aggr ctrl
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtrpc_vlan_trkVlanAggrCtrl_get(uint32 unit, rtk_trk_t tid, rtk_vlan_aggrCtrl_t *pVlanAggrCtrl)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pVlanAggrCtrl), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.tid, &tid, sizeof(rtk_trk_t));
    GETSOCKOPT(RTDRV_VLAN_TRKVLANAGGRCTRL_GET, &cfg, rtdrv_vlanCfg_t, 1);
    osal_memcpy(pVlanAggrCtrl, &cfg.vlanAggrCtrl, sizeof(rtk_vlan_aggrCtrl_t));

    return RT_ERR_OK;
}   /* end of rtk_vlan_trkVlanAggrCtrl_get */

/* Function Name:
 *      rtk_vlan_trkVlanAggrCtrl_set
 * Description:
 *      Set trunk vlan aggragation configuration
 * Input:
 *      unit         - unit id
 *      tid          - trunk id
 *      vlanAggrCtrl - vlan-aggr ctrl
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_TRUNK_ID - invalid trunk id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtrpc_vlan_trkVlanAggrCtrl_set(uint32 unit, rtk_trk_t tid, rtk_vlan_aggrCtrl_t vlanAggrCtrl)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.tid, &tid, sizeof(rtk_trk_t));
    osal_memcpy(&cfg.vlanAggrCtrl, &vlanAggrCtrl, sizeof(rtk_vlan_aggrCtrl_t));
    SETSOCKOPT(RTDRV_VLAN_TRKVLANAGGRCTRL_SET, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vlan_trkVlanAggrCtrl_set */

int32
rtrpc_vlan_intfId_get(uint32 unit, rtk_vlan_t vid, rtk_intf_id_t *pIntfId)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntfId), RT_ERR_NULL_POINTER);

    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.vid, &vid, sizeof(rtk_vlan_t));
    GETSOCKOPT(RTDRV_VLAN_INTFID_GET, &cfg, rtdrv_vlanCfg_t, 1);
    memcpy(pIntfId, &cfg.intfId, sizeof(rtk_intf_id_t));

    return RT_ERR_OK;
}   /* end of rtrpc_vlan_intfId_get */

int32
rtrpc_vlan_intfId_set(uint32 unit, rtk_vlan_t vid, rtk_intf_id_t intfId)
{
    rtdrv_vlanCfg_t cfg;
    uint32 master_view_unit = unit;

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_vlanCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.vid, &vid, sizeof(rtk_vlan_t));
    memcpy(&cfg.intfId, &intfId, sizeof(rtk_intf_id_t));
    SETSOCKOPT(RTDRV_VLAN_INTFID_SET, &cfg, rtdrv_vlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_vlan_intfId_set */

int32
rtrpc_vlanMapper_init(dal_mapper_t *pMapper)
{
    pMapper->vlan_create = rtrpc_vlan_create;
    pMapper->vlan_destroy = rtrpc_vlan_destroy;
    pMapper->vlan_destroyAll = rtrpc_vlan_destroyAll;
    pMapper->vlan_port_add = rtrpc_vlan_port_add;
    pMapper->vlan_port_del = rtrpc_vlan_port_del;
    pMapper->vlan_port_get = rtrpc_vlan_port_get;
    pMapper->vlan_port_set = rtrpc_vlan_port_set;
    pMapper->vlan_mcastGroup_get = rtrpc_vlan_mcastGroup_get;
    pMapper->vlan_mcastGroup_set = rtrpc_vlan_mcastGroup_set;
    pMapper->vlan_svlMode_get = rtrpc_vlan_svlMode_get;
    pMapper->vlan_svlMode_set = rtrpc_vlan_svlMode_set;
    pMapper->vlan_svlFid_get = rtrpc_vlan_svlFid_get;
    pMapper->vlan_svlFid_set = rtrpc_vlan_svlFid_set;
    pMapper->vlan_stg_get = rtrpc_vlan_stg_get;
    pMapper->vlan_stg_set = rtrpc_vlan_stg_set;
    pMapper->vlan_l2LookupSvlFid_get = rtrpc_vlan_l2LookupSvlFid_get;
    pMapper->vlan_l2LookupSvlFid_set = rtrpc_vlan_l2LookupSvlFid_set;
    pMapper->vlan_l2LookupMode_get = rtrpc_vlan_l2LookupMode_get;
    pMapper->vlan_l2LookupMode_set = rtrpc_vlan_l2LookupMode_set;
    pMapper->vlan_groupMask_get = rtrpc_vlan_groupMask_get;
    pMapper->vlan_groupMask_set = rtrpc_vlan_groupMask_set;
    pMapper->vlan_profileIdx_get = rtrpc_vlan_profileIdx_get;
    pMapper->vlan_profileIdx_set = rtrpc_vlan_profileIdx_set;
    pMapper->vlan_profile_get = rtrpc_vlan_profile_get;
    pMapper->vlan_profile_set = rtrpc_vlan_profile_set;
    pMapper->vlan_portFwdVlan_get = rtrpc_vlan_portFwdVlan_get ;
    pMapper->vlan_portFwdVlan_set = rtrpc_vlan_portFwdVlan_set ;
    pMapper->vlan_portAcceptFrameType_get = rtrpc_vlan_portAcceptFrameType_get;
    pMapper->vlan_portAcceptFrameType_set = rtrpc_vlan_portAcceptFrameType_set;
    pMapper->vlan_portIgrFilter_get = rtrpc_vlan_portIgrFilter_get;
    pMapper->vlan_portIgrFilter_set = rtrpc_vlan_portIgrFilter_set;
    pMapper->vlan_portEgrFilterEnable_get = rtrpc_vlan_portEgrFilterEnable_get;
    pMapper->vlan_portEgrFilterEnable_set = rtrpc_vlan_portEgrFilterEnable_set;
    pMapper->vlan_mcastLeakyEnable_get = rtrpc_vlan_mcastLeakyEnable_get;
    pMapper->vlan_mcastLeakyEnable_set = rtrpc_vlan_mcastLeakyEnable_set;
    pMapper->vlan_portPvidMode_get = rtrpc_vlan_portPvidMode_get;
    pMapper->vlan_portPvidMode_set = rtrpc_vlan_portPvidMode_set;
    pMapper->vlan_portPvid_get = rtrpc_vlan_portPvid_get;
    pMapper->vlan_portPvid_set = rtrpc_vlan_portPvid_set;
    pMapper->vlan_portPrivateVlanEnable_get = rtrpc_vlan_portPrivateVlanEnable_get;
    pMapper->vlan_portPrivateVlanEnable_set = rtrpc_vlan_portPrivateVlanEnable_set;
    pMapper->vlan_protoGroup_get = rtrpc_vlan_protoGroup_get;
    pMapper->vlan_protoGroup_set = rtrpc_vlan_protoGroup_set;
    pMapper->vlan_portProtoVlan_get = rtrpc_vlan_portProtoVlan_get;
    pMapper->vlan_portProtoVlan_set = rtrpc_vlan_portProtoVlan_set;
    pMapper->vlan_macBasedVlan_get = rtrpc_vlan_macBasedVlan_get;
    pMapper->vlan_macBasedVlan_set = rtrpc_vlan_macBasedVlan_set;
    pMapper->vlan_macBasedVlanWithMsk_get = rtrpc_vlan_macBasedVlanWithMsk_get;
    pMapper->vlan_macBasedVlanWithMsk_set = rtrpc_vlan_macBasedVlanWithMsk_set;
    pMapper->vlan_macBasedVlanWithPort_get = rtrpc_vlan_macBasedVlanWithPort_get;
    pMapper->vlan_macBasedVlanWithPort_set = rtrpc_vlan_macBasedVlanWithPort_set;
    pMapper->vlan_portMacBasedVlanEnable_get = rtrpc_vlan_portMacBasedVlanEnable_get;
    pMapper->vlan_portMacBasedVlanEnable_set = rtrpc_vlan_portMacBasedVlanEnable_set;
    pMapper->vlan_macBasedVlanEntry_get = rtrpc_vlan_macBasedVlanEntry_get;
    pMapper->vlan_macBasedVlanEntry_set = rtrpc_vlan_macBasedVlanEntry_set;
    pMapper->vlan_macBasedVlanEntry_add = rtrpc_vlan_macBasedVlanEntry_add;
    pMapper->vlan_macBasedVlanEntry_del = rtrpc_vlan_macBasedVlanEntry_del;
    pMapper->vlan_ipSubnetBasedVlan_get = rtrpc_vlan_ipSubnetBasedVlan_get;
    pMapper->vlan_ipSubnetBasedVlan_set = rtrpc_vlan_ipSubnetBasedVlan_set;
    pMapper->vlan_ipSubnetBasedVlanWithPort_get = rtrpc_vlan_ipSubnetBasedVlanWithPort_get;
    pMapper->vlan_ipSubnetBasedVlanWithPort_set = rtrpc_vlan_ipSubnetBasedVlanWithPort_set;
    pMapper->vlan_portIpSubnetBasedVlanEnable_get = rtrpc_vlan_portIpSubnetBasedVlanEnable_get;
    pMapper->vlan_portIpSubnetBasedVlanEnable_set = rtrpc_vlan_portIpSubnetBasedVlanEnable_set;
    pMapper->vlan_ipSubnetBasedVlanEntry_get = rtrpc_vlan_ipSubnetBasedVlanEntry_get;
    pMapper->vlan_ipSubnetBasedVlanEntry_set = rtrpc_vlan_ipSubnetBasedVlanEntry_set;
    pMapper->vlan_ipSubnetBasedVlanEntry_add = rtrpc_vlan_ipSubnetBasedVlanEntry_add;
    pMapper->vlan_ipSubnetBasedVlanEntry_del = rtrpc_vlan_ipSubnetBasedVlanEntry_del;
    pMapper->vlan_tpidEntry_get = rtrpc_vlan_tpidEntry_get;
    pMapper->vlan_tpidEntry_set = rtrpc_vlan_tpidEntry_set;
    pMapper->vlan_portIgrTpid_get = rtrpc_vlan_portIgrTpid_get;
    pMapper->vlan_portIgrTpid_set = rtrpc_vlan_portIgrTpid_set;
    pMapper->vlan_portEgrTpid_get = rtrpc_vlan_portEgrTpid_get;
    pMapper->vlan_portEgrTpid_set = rtrpc_vlan_portEgrTpid_set;
    pMapper->vlan_portEgrTpidSrc_get = rtrpc_vlan_portEgrTpidSrc_get;
    pMapper->vlan_portEgrTpidSrc_set = rtrpc_vlan_portEgrTpidSrc_set;
    pMapper->vlan_portIgrExtraTagEnable_get = rtrpc_vlan_portIgrExtraTagEnable_get;
    pMapper->vlan_portIgrExtraTagEnable_set = rtrpc_vlan_portIgrExtraTagEnable_set;
    pMapper->vlan_portEgrTagSts_get = rtrpc_vlan_portEgrTagSts_get;
    pMapper->vlan_portEgrTagSts_set = rtrpc_vlan_portEgrTagSts_set;
    pMapper->vlan_portIgrVlanTransparentEnable_get = rtrpc_vlan_portIgrVlanTransparentEnable_get;
    pMapper->vlan_portIgrVlanTransparentEnable_set = rtrpc_vlan_portIgrVlanTransparentEnable_set;
    pMapper->vlan_portEgrVlanTransparentEnable_get = rtrpc_vlan_portEgrVlanTransparentEnable_get;
    pMapper->vlan_portEgrVlanTransparentEnable_set = rtrpc_vlan_portEgrVlanTransparentEnable_set;
    pMapper->vlan_igrVlanCnvtBlkMode_get = rtrpc_vlan_igrVlanCnvtBlkMode_get;
    pMapper->vlan_igrVlanCnvtBlkMode_set = rtrpc_vlan_igrVlanCnvtBlkMode_set;
    pMapper->vlan_igrVlanCnvtEntry_get = rtrpc_vlan_igrVlanCnvtEntry_get;
    pMapper->vlan_igrVlanCnvtEntry_set = rtrpc_vlan_igrVlanCnvtEntry_set;
    pMapper->vlan_portIgrVlanCnvtEnable_get = rtrpc_vlan_portIgrVlanCnvtEnable_get;
    pMapper->vlan_portIgrVlanCnvtEnable_set = rtrpc_vlan_portIgrVlanCnvtEnable_set;
    pMapper->vlan_egrVlanCnvtDblTagEnable_get = rtrpc_vlan_egrVlanCnvtDblTagEnable_get;
    pMapper->vlan_egrVlanCnvtDblTagEnable_set = rtrpc_vlan_egrVlanCnvtDblTagEnable_set;
    pMapper->vlan_egrVlanCnvtVidSource_get = rtrpc_vlan_egrVlanCnvtVidSource_get;
    pMapper->vlan_egrVlanCnvtVidSource_set = rtrpc_vlan_egrVlanCnvtVidSource_set;
    pMapper->vlan_egrVlanCnvtEntry_get = rtrpc_vlan_egrVlanCnvtEntry_get;
    pMapper->vlan_egrVlanCnvtEntry_set = rtrpc_vlan_egrVlanCnvtEntry_set;
    pMapper->vlan_portEgrVlanCnvtEnable_get = rtrpc_vlan_portEgrVlanCnvtEnable_get;
    pMapper->vlan_portEgrVlanCnvtEnable_set = rtrpc_vlan_portEgrVlanCnvtEnable_set;
    pMapper->vlan_aggrEnable_get = rtrpc_vlan_aggrEnable_get;
    pMapper->vlan_aggrEnable_set = rtrpc_vlan_aggrEnable_set;
    pMapper->vlan_portVlanAggrEnable_get = rtrpc_vlan_portVlanAggrEnable_get;
    pMapper->vlan_portVlanAggrEnable_set = rtrpc_vlan_portVlanAggrEnable_set;
    pMapper->vlan_portVlanAggrPriEnable_get = rtrpc_vlan_portVlanAggrPriEnable_get;
    pMapper->vlan_portVlanAggrPriEnable_set = rtrpc_vlan_portVlanAggrPriEnable_set;
    pMapper->vlan_portVlanAggrCtrl_get = rtrpc_vlan_portVlanAggrCtrl_get;
    pMapper->vlan_portVlanAggrCtrl_set = rtrpc_vlan_portVlanAggrCtrl_set;
    pMapper->vlan_leakyStpFilter_get = rtrpc_vlan_leakyStpFilter_get;
    pMapper->vlan_leakyStpFilter_set = rtrpc_vlan_leakyStpFilter_set;
    pMapper->vlan_except_get = rtrpc_vlan_except_get;
    pMapper->vlan_except_set = rtrpc_vlan_except_set;
    pMapper->vlan_portIgrCnvtDfltAct_get = rtrpc_vlan_portIgrCnvtDfltAct_get;
    pMapper->vlan_portIgrCnvtDfltAct_set = rtrpc_vlan_portIgrCnvtDfltAct_set;
    pMapper->vlan_portIgrVlanCnvtLuMisAct_get = rtrpc_vlan_portIgrVlanCnvtLuMisAct_get;
    pMapper->vlan_portIgrVlanCnvtLuMisAct_set = rtrpc_vlan_portIgrVlanCnvtLuMisAct_set;
    pMapper->vlan_portEgrVlanCnvtLuMisAct_get = rtrpc_vlan_portEgrVlanCnvtLuMisAct_get;
    pMapper->vlan_portEgrVlanCnvtLuMisAct_set = rtrpc_vlan_portEgrVlanCnvtLuMisAct_set;
    pMapper->vlan_portIgrTagKeepType_get = rtrpc_vlan_portIgrTagKeepType_get;
    pMapper->vlan_portIgrTagKeepType_set = rtrpc_vlan_portIgrTagKeepType_set;
    pMapper->vlan_portEgrTagKeepType_get = rtrpc_vlan_portEgrTagKeepType_get;
    pMapper->vlan_portEgrTagKeepType_set = rtrpc_vlan_portEgrTagKeepType_set;
    pMapper->vlan_portEgrVlanCnvtVidSource_get = rtrpc_vlan_portEgrVlanCnvtVidSource_get;
    pMapper->vlan_portEgrVlanCnvtVidSource_set = rtrpc_vlan_portEgrVlanCnvtVidSource_set;
    pMapper->vlan_portEgrVlanCnvtVidTarget_get = rtrpc_vlan_portEgrVlanCnvtVidTarget_get;
    pMapper->vlan_portEgrVlanCnvtVidTarget_set = rtrpc_vlan_portEgrVlanCnvtVidTarget_set;
    pMapper->vlan_igrVlanCnvtHitIndication_get = rtrpc_vlan_igrVlanCnvtHitIndication_get;
    pMapper->vlan_egrVlanCnvtHitIndication_get = rtrpc_vlan_egrVlanCnvtHitIndication_get;
    pMapper->vlan_igrVlanCnvtEntry_delAll = rtrpc_vlan_igrVlanCnvtEntry_delAll;
    pMapper->vlan_egrVlanCnvtEntry_delAll = rtrpc_vlan_egrVlanCnvtEntry_delAll;
    pMapper->vlan_portIgrVlanCnvtRangeCheckSet_get = rtrpc_vlan_portIgrVlanCnvtRangeCheckSet_get;
    pMapper->vlan_portIgrVlanCnvtRangeCheckSet_set = rtrpc_vlan_portIgrVlanCnvtRangeCheckSet_set;
    pMapper->vlan_igrVlanCnvtRangeCheckEntry_get = rtrpc_vlan_igrVlanCnvtRangeCheckEntry_get;
    pMapper->vlan_igrVlanCnvtRangeCheckEntry_set = rtrpc_vlan_igrVlanCnvtRangeCheckEntry_set;
    pMapper->vlan_portEgrVlanCnvtRangeCheckSet_get = rtrpc_vlan_portEgrVlanCnvtRangeCheckSet_get;
    pMapper->vlan_portEgrVlanCnvtRangeCheckSet_set = rtrpc_vlan_portEgrVlanCnvtRangeCheckSet_set;
    pMapper->vlan_egrVlanCnvtRangeCheckEntry_get = rtrpc_vlan_egrVlanCnvtRangeCheckEntry_get;
    pMapper->vlan_egrVlanCnvtRangeCheckEntry_set = rtrpc_vlan_egrVlanCnvtRangeCheckEntry_set;
    pMapper->vlan_ecidPmsk_add = rtrpc_vlan_ecidPmsk_add;
    pMapper->vlan_ecidPmsk_del = rtrpc_vlan_ecidPmsk_del;
    pMapper->vlan_ecidPmsk_get = rtrpc_vlan_ecidPmsk_get;
    pMapper->vlan_ecidPmskNextValid_get = rtrpc_vlan_ecidPmskNextValid_get;
    pMapper->vlan_trkVlanAggrEnable_get = rtrpc_vlan_trkVlanAggrEnable_get;
    pMapper->vlan_trkVlanAggrEnable_set = rtrpc_vlan_trkVlanAggrEnable_set;
    pMapper->vlan_trkVlanAggrPriEnable_get = rtrpc_vlan_trkVlanAggrPriEnable_get;
    pMapper->vlan_trkVlanAggrPriEnable_set = rtrpc_vlan_trkVlanAggrPriEnable_set;
    pMapper->vlan_trkVlanAggrCtrl_get = rtrpc_vlan_trkVlanAggrCtrl_get;
    pMapper->vlan_trkVlanAggrCtrl_set = rtrpc_vlan_trkVlanAggrCtrl_set;
    pMapper->vlan_intfId_get = rtrpc_vlan_intfId_get;
    pMapper->vlan_intfId_set = rtrpc_vlan_intfId_set;

    return RT_ERR_OK;
}

