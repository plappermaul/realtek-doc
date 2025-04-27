/*
 * Copyright (C) 2009-2017 Realtek Semiconductor Corp.
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
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) vxlan
 *
 */

#include <dal/rtrpc/rtrpc_vxlan.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>


int32
rtrpc_vxlan_init(uint32 unit)
{
    rtdrv_vxlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    SETSOCKOPT(RTDRV_VXLAN_INIT, &cfg, rtdrv_vxlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vxlan_init */

int32
rtrpc_vxlan_vni_add(uint32 unit, rtk_vxlan_vniEntry_t *pEntry)
{
    rtdrv_vxlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.entry, pEntry, sizeof(rtk_vxlan_vniEntry_t));
    SETSOCKOPT(RTDRV_VXLAN_VNI_ADD, &cfg, rtdrv_vxlanCfg_t, 1);
    osal_memcpy(pEntry, &cfg.entry, sizeof(rtk_vxlan_vniEntry_t));

    return RT_ERR_OK;
}   /* end of rtk_vxlan_vni_add */

int32
rtrpc_vxlan_vni_del(uint32 unit, rtk_vxlan_vniEntry_t *pEntry)
{
    rtdrv_vxlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.entry, pEntry, sizeof(rtk_vxlan_vniEntry_t));
    SETSOCKOPT(RTDRV_VXLAN_VNI_DEL, &cfg, rtdrv_vxlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vxlan_vni_del */

int32
rtrpc_vxlan_vni_delAll(uint32 unit)
{
    rtdrv_vxlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    SETSOCKOPT(RTDRV_VXLAN_VNI_DELALL, &cfg, rtdrv_vxlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vxlan_vni_delAll */

int32
rtrpc_vxlan_vni_get(uint32 unit, rtk_vxlan_vniEntry_t *pEntry)
{
    rtdrv_vxlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.entry, pEntry, sizeof(rtk_vxlan_vniEntry_t));
    GETSOCKOPT(RTDRV_VXLAN_VNI_GET, &cfg, rtdrv_vxlanCfg_t, 1);
    osal_memcpy(pEntry, &cfg.entry, sizeof(rtk_vxlan_vniEntry_t));

    return RT_ERR_OK;
}   /* end of rtk_vxlan_vni_get */

int32
rtrpc_vxlan_vni_set(uint32 unit, rtk_vxlan_vniEntry_t *pEntry)
{
    rtdrv_vxlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.entry, pEntry, sizeof(rtk_vxlan_vniEntry_t));
    SETSOCKOPT(RTDRV_VXLAN_VNI_SET, &cfg, rtdrv_vxlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vxlan_vni_set */

int32
rtrpc_vxlan_vni_getNext(uint32 unit, int32 *pBase, rtk_vxlan_vniEntry_t *pEntry)
{
    rtdrv_vxlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBase), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.base, pBase, sizeof(int32));
    GETSOCKOPT(RTDRV_VXLAN_VNI_GETNEXT, &cfg, rtdrv_vxlanCfg_t, 1);
    osal_memcpy(pBase, &cfg.base, sizeof(int32));
    osal_memcpy(pEntry, &cfg.entry, sizeof(rtk_vxlan_vniEntry_t));

    return RT_ERR_OK;
}   /* end of rtk_vxlan_vni_getNext */

int32
rtrpc_vxlan_globalCtrl_get(uint32 unit, rtk_vxlan_globalCtrlType_t type, int32 *pArg)
{
    rtdrv_vxlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pArg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.type, &type, sizeof(rtk_vxlan_globalCtrlType_t));
    GETSOCKOPT(RTDRV_VXLAN_GLOBALCTRL_GET, &cfg, rtdrv_vxlanCfg_t, 1);
    osal_memcpy(pArg, &cfg.arg, sizeof(int32));

    return RT_ERR_OK;
}   /* end of rtk_vxlan_globalCtrl_get */

int32
rtrpc_vxlan_globalCtrl_set(uint32 unit, rtk_vxlan_globalCtrlType_t type, int32 arg)
{
    rtdrv_vxlanCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.type, &type, sizeof(rtk_vxlan_globalCtrlType_t));
    osal_memcpy(&cfg.arg, &arg, sizeof(int32));
    SETSOCKOPT(RTDRV_VXLAN_GLOBALCTRL_SET, &cfg, rtdrv_vxlanCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_vxlan_globalCtrl_set */



int32
rtrpc_vxlanMapper_init(dal_mapper_t *pMapper)
{
    pMapper->vxlan_vni_add = rtrpc_vxlan_vni_add;
    pMapper->vxlan_vni_del = rtrpc_vxlan_vni_del;
    pMapper->vxlan_vni_delAll = rtrpc_vxlan_vni_delAll;
    pMapper->vxlan_vni_get = rtrpc_vxlan_vni_get;
    pMapper->vxlan_vni_set = rtrpc_vxlan_vni_set;
    pMapper->vxlan_vni_getNext = rtrpc_vxlan_vni_getNext;
    pMapper->vxlan_globalCtrl_get = rtrpc_vxlan_globalCtrl_get;
    pMapper->vxlan_globalCtrl_set = rtrpc_vxlan_globalCtrl_set;
    return RT_ERR_OK;
}
