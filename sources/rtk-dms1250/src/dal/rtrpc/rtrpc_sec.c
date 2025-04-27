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
 * $Revision: 79582 $
 * $Date: 2017-06-13 16:50:29 +0800 (Tue, 13 Jun 2017) $
 *
 * Purpose : Definition those public security APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Attack prevention
 *
 */

#include <rtk/sec.h>
#include <dal/rtrpc/rtrpc_sec.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>


int32 rtrpc_sec_portAttackPrevent_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            *pAction)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    sec_cfg.port = port;
    sec_cfg.attack_type = attack_type;
    GETSOCKOPT(RTDRV_SEC_PORT_ATTACK_PREVENT_GET, &sec_cfg, rtdrv_secCfg_t, 1);
    *pAction = sec_cfg.action;

    return RT_ERR_OK;
}

int32 rtrpc_sec_portAttackPrevent_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            action)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    sec_cfg.port = port;
    sec_cfg.attack_type = attack_type;
    sec_cfg.action = action;
    SETSOCKOPT(RTDRV_SEC_PORT_ATTACK_PREVENT_SET, &sec_cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_sec_portAttackPreventEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_enable_t            *pEnable)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    sec_cfg.port = port;
    GETSOCKOPT(RTDRV_SEC_PORT_ATTACK_PREVENT_ENABLE_GET, &sec_cfg, rtdrv_secCfg_t, 1);
    *pEnable = sec_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_sec_portAttackPreventEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_enable_t            enable)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    sec_cfg.port = port;
    sec_cfg.enable = enable;
    SETSOCKOPT(RTDRV_SEC_PORT_ATTACK_PREVENT_ENABLE_SET, &sec_cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_sec_attackPreventAction_get(
    uint32                  unit,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            *pAction)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    sec_cfg.attack_type = attack_type;
    GETSOCKOPT(RTDRV_SEC_ATTACK_PREVENT_GET, &sec_cfg, rtdrv_secCfg_t, 1);
    *pAction = sec_cfg.action;

    return RT_ERR_OK;
}

int32 rtrpc_sec_attackPreventAction_set(
    uint32                  unit,
    rtk_sec_attackType_t    attack_type,
    rtk_action_t            action)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    sec_cfg.attack_type = attack_type;
    sec_cfg.action = action;
    SETSOCKOPT(RTDRV_SEC_ATTACK_PREVENT_SET, &sec_cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_sec_minIPv6FragLen_get(uint32 unit, uint32 *pLength)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    GETSOCKOPT(RTDRV_SEC_MIN_IPV6_FRAG_LEN_GET, &sec_cfg, rtdrv_secCfg_t, 1);
    *pLength = sec_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_sec_minIPv6FragLen_set(uint32 unit, uint32 length)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    sec_cfg.data = length;
    SETSOCKOPT(RTDRV_SEC_MIN_IPV6_FRAG_LEN_SET, &sec_cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_sec_maxPingLen_get(uint32 unit, uint32 *pLength)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    GETSOCKOPT(RTDRV_SEC_MAX_PING_LEN_GET, &sec_cfg, rtdrv_secCfg_t, 1);
    *pLength = sec_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_sec_maxPingLen_set(uint32 unit, uint32 length)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    sec_cfg.data = length;
    SETSOCKOPT(RTDRV_SEC_MAX_PING_LEN_SET, &sec_cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_sec_minTCPHdrLen_get(uint32 unit, uint32 *pLength)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    GETSOCKOPT(RTDRV_SEC_MIN_TCP_HDR_LEN_GET, &sec_cfg, rtdrv_secCfg_t, 1);
    *pLength = sec_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_sec_minTCPHdrLen_set(uint32 unit, uint32 length)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    sec_cfg.data = length;
    SETSOCKOPT(RTDRV_SEC_MIN_TCP_HDR_LEN_SET, &sec_cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_sec_smurfNetmaskLen_get(uint32 unit, uint32 *pLength)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    GETSOCKOPT(RTDRV_SEC_SMURF_NETMASK_LEN_GET, &sec_cfg, rtdrv_secCfg_t, 1);
    *pLength = sec_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_sec_smurfNetmaskLen_set(uint32 unit, uint32 length)
{
    rtdrv_secCfg_t sec_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sec_cfg, 0, sizeof(rtdrv_secCfg_t));
    sec_cfg.unit = unit;
    sec_cfg.data = length;
    SETSOCKOPT(RTDRV_SEC_SMURF_NETMASK_LEN_SET, &sec_cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_sec_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget)
{
    rtdrv_secCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTarget), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_SEC_TRAPTARGET_GET, &cfg, rtdrv_secCfg_t, 1);
    osal_memcpy(pTarget, &cfg.target, sizeof(rtk_trapTarget_t));

    return RT_ERR_OK;
}   /* end of rtk_sec_trapTarget_get */

int32
rtrpc_sec_trapTarget_set(uint32 unit, rtk_trapTarget_t target)
{
    rtdrv_secCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.target, &target, sizeof(rtk_trapTarget_t));
    SETSOCKOPT(RTDRV_SEC_TRAPTARGET_SET, &cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sec_trapTarget_set */

int32
rtrpc_sec_ipMacBindAction_get(uint32 unit, rtk_action_t *pLumisAct, rtk_action_t *pMatchAct, rtk_action_t *pMismatchAct)
{
    rtdrv_secCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLumisAct), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMatchAct), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pMismatchAct), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_SEC_IPMACBINDACTION_GET, &cfg, rtdrv_secCfg_t, 1);
    osal_memcpy(pLumisAct, &cfg.lumisAct, sizeof(rtk_action_t));
    osal_memcpy(pMatchAct, &cfg.matchAct, sizeof(rtk_action_t));
    osal_memcpy(pMismatchAct, &cfg.mismatchAct, sizeof(rtk_action_t));

    return RT_ERR_OK;
}   /* end of rtk_sec_ipMacBindAction_get */

int32
rtrpc_sec_ipMacBindAction_set(uint32 unit, rtk_action_t lumisAct, rtk_action_t matchAct, rtk_action_t mismatchAct)
{
    rtdrv_secCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.lumisAct, &lumisAct, sizeof(rtk_action_t));
    osal_memcpy(&cfg.matchAct, &matchAct, sizeof(rtk_action_t));
    osal_memcpy(&cfg.mismatchAct, &mismatchAct, sizeof(rtk_action_t));
    SETSOCKOPT(RTDRV_SEC_IPMACBINDACTION_SET, &cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sec_ipMacBindAction_set */

int32
rtrpc_sec_portIpMacBindEnable_get(uint32 unit, rtk_port_t port, rtk_sec_ipMacBindPktType_t type, rtk_enable_t *pEnable)
{
    rtdrv_secCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.type, &type, sizeof(rtk_sec_ipMacBindPktType_t));
    GETSOCKOPT(RTDRV_SEC_PORTIPMACBINDENABLE_GET, &cfg, rtdrv_secCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_sec_portIpMacBindEnable_get */

int32
rtrpc_sec_portIpMacBindEnable_set(uint32 unit, rtk_port_t port, rtk_sec_ipMacBindPktType_t type, rtk_enable_t enable)
{
    rtdrv_secCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.type, &type, sizeof(rtk_sec_ipMacBindPktType_t));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_SEC_PORTIPMACBINDENABLE_SET, &cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sec_portIpMacBindEnable_set */

int32
rtrpc_sec_ipMacBindEntry_add(uint32 unit, rtk_sec_ipMacBindEntry_t *pEntry)
{
    rtdrv_secCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.entry, pEntry, sizeof(rtk_sec_ipMacBindEntry_t));
    SETSOCKOPT(RTDRV_SEC_IPMACBINDENTRY_ADD, &cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sec_ipMacBindEntry_add */

int32
rtrpc_sec_ipMacBindEntry_del(uint32 unit, rtk_sec_ipMacBindEntry_t *pEntry)
{
    rtdrv_secCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.entry, pEntry, sizeof(rtk_sec_ipMacBindEntry_t));
    SETSOCKOPT(RTDRV_SEC_IPMACBINDENTRY_DEL, &cfg, rtdrv_secCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_sec_ipMacBindEntry_del */

int32
rtrpc_sec_ipMacBindEntry_getNext(uint32 unit, int32 *pBase, rtk_sec_ipMacBindEntry_t *pEntry)
{
    rtdrv_secCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBase), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.base, pBase, sizeof(int32));
    GETSOCKOPT(RTDRV_SEC_IPMACBINDENTRY_GETNEXT, &cfg, rtdrv_secCfg_t, 1);
    osal_memcpy(pBase, &cfg.base, sizeof(int32));
    osal_memcpy(pEntry, &cfg.entry, sizeof(rtk_sec_ipMacBindEntry_t));

    return RT_ERR_OK;
}   /* end of rtk_sec_ipMacBindEntry_getNext */

int32
rtrpc_sec_attackPreventHit_get(uint32 unit,
    rtk_sec_attackType_t attack_type, uint32 *pHit)
{
    rtdrv_secCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pHit), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_secCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.attack_type, &attack_type, sizeof(rtk_sec_attackType_t));
    GETSOCKOPT(RTDRV_SEC_ATTACKPREVENTHIT_GET, &cfg, rtdrv_secCfg_t, 1);
    osal_memcpy(pHit, &cfg.data, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_sec_attackPreventHit_get */


int32
rtrpc_secMapper_init(dal_mapper_t *pMapper)
{
    pMapper->sec_portAttackPrevent_get = rtrpc_sec_portAttackPrevent_get;
    pMapper->sec_portAttackPrevent_set = rtrpc_sec_portAttackPrevent_set;
    pMapper->sec_portAttackPreventEnable_get = rtrpc_sec_portAttackPreventEnable_get;
    pMapper->sec_portAttackPreventEnable_set = rtrpc_sec_portAttackPreventEnable_set;
    pMapper->sec_attackPreventAction_get = rtrpc_sec_attackPreventAction_get;
    pMapper->sec_attackPreventAction_set = rtrpc_sec_attackPreventAction_set;
    pMapper->sec_minIPv6FragLen_get = rtrpc_sec_minIPv6FragLen_get;
    pMapper->sec_minIPv6FragLen_set = rtrpc_sec_minIPv6FragLen_set;
    pMapper->sec_maxPingLen_get = rtrpc_sec_maxPingLen_get;
    pMapper->sec_maxPingLen_set = rtrpc_sec_maxPingLen_set;
    pMapper->sec_minTCPHdrLen_get = rtrpc_sec_minTCPHdrLen_get;
    pMapper->sec_minTCPHdrLen_set = rtrpc_sec_minTCPHdrLen_set;
    pMapper->sec_smurfNetmaskLen_get = rtrpc_sec_smurfNetmaskLen_get;
    pMapper->sec_smurfNetmaskLen_set = rtrpc_sec_smurfNetmaskLen_set;
    pMapper->sec_trapTarget_get = rtrpc_sec_trapTarget_get;
    pMapper->sec_trapTarget_set = rtrpc_sec_trapTarget_set;
    pMapper->sec_ipMacBindAction_get = rtrpc_sec_ipMacBindAction_get;
    pMapper->sec_ipMacBindAction_set = rtrpc_sec_ipMacBindAction_set;
    pMapper->sec_portIpMacBindEnable_get = rtrpc_sec_portIpMacBindEnable_get;
    pMapper->sec_portIpMacBindEnable_set = rtrpc_sec_portIpMacBindEnable_set;
    pMapper->sec_ipMacBindEntry_add = rtrpc_sec_ipMacBindEntry_add;
    pMapper->sec_ipMacBindEntry_del = rtrpc_sec_ipMacBindEntry_del;
    pMapper->sec_ipMacBindEntry_getNext = rtrpc_sec_ipMacBindEntry_getNext;
    pMapper->sec_attackPreventHit_get = rtrpc_sec_attackPreventHit_get;
    return RT_ERR_OK;
}
