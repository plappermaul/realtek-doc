/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 83769 $
 * $Date: 2017-11-27 19:33:02 +0800 (Mon, 27 Nov 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) port
 *
 */

#include <rtk/port.h>
#include <dal/rtrpc/rtrpc_port.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 rtrpc_port_link_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_LINK_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pStatus = port_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_port_linkMedia_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus, rtk_port_media_t *pMedia)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_LINKMEDIA_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pStatus = port_cfg.data;
    *pMedia = port_cfg.media;

    return RT_ERR_OK;
}

int32 rtrpc_port_speedDuplex_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex)
{
    rtdrv_port_speedDuplex_t sd_status;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&sd_status, 0, sizeof(rtdrv_port_speedDuplex_t));
    sd_status.unit = unit;
    sd_status.port = port;
    GETSOCKOPT(RTDRV_PORT_SPEED_DUPLEX_GET, &sd_status, rtdrv_port_speedDuplex_t, 1);
    *pSpeed = sd_status.speed;
    *pDuplex = sd_status.duplex;

    return RT_ERR_OK;
}

int32 rtrpc_port_flowctrl_get(uint32 unit, rtk_port_t port, uint32 *pTxStatus, uint32 *pRxStatus)
{
    rtdrv_port_flowctrl_t fc_status;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&fc_status, 0, sizeof(rtdrv_port_flowctrl_t));
    fc_status.unit = unit;
    fc_status.port = port;
    GETSOCKOPT(RTDRV_PORT_FLOW_CTRL_GET, &fc_status, rtdrv_port_flowctrl_t, 1);
    *pTxStatus = fc_status.tx_status;
    *pRxStatus = fc_status.rx_status;

    return RT_ERR_OK;
}

int32 rtrpc_port_cpuPortId_get(uint32 unit, rtk_port_t *pPort)
{
    rtdrv_unitCfg_t unit_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&unit_cfg, 0, sizeof(rtdrv_unitCfg_t));
    unit_cfg.unit = unit;
    GETSOCKOPT(RTDRV_PORT_CPU_PORT_ID_GET, &unit_cfg, rtdrv_unitCfg_t, 1);
    *pPort = unit_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_port_phyAutoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnabled)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_EN_AUTONEGO_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pEnabled = port_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_port_phyAutoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enabled)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = enabled;
    SETSOCKOPT(RTDRV_PORT_EN_AUTONEGO_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_port_phyAutoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    rtdrv_port_autoNegoAbility_t an_ability;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&an_ability, 0, sizeof(rtdrv_port_autoNegoAbility_t));
    an_ability.unit = unit;
    an_ability.port = port;
    GETSOCKOPT(RTDRV_PORT_AUTONEGO_ABIL_GET, &an_ability, rtdrv_port_autoNegoAbility_t, 1);
    osal_memcpy(pAbility, &an_ability.ability, sizeof(rtk_port_phy_ability_t));

    return RT_ERR_OK;
}

int32 rtrpc_port_phyAutoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    rtdrv_port_autoNegoAbility_t an_ability;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&an_ability, 0, sizeof(rtdrv_port_autoNegoAbility_t));
    an_ability.unit = unit;
    an_ability.port = port;
    osal_memcpy(&an_ability.ability, pAbility, sizeof(rtk_port_phy_ability_t));
    SETSOCKOPT(RTDRV_PORT_AUTONEGO_ABIL_SET, &an_ability, rtdrv_port_autoNegoAbility_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_port_phyForceModeAbility_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex, rtk_enable_t *pFlowControl)
{
    rtdrv_port_forceModeAbility_t fm_ability;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&fm_ability, 0, sizeof(rtdrv_port_forceModeAbility_t));
    fm_ability.unit = unit;
    fm_ability.port = port;
    GETSOCKOPT(RTDRV_PORT_FORCE_MODE_ABIL_GET, &fm_ability, rtdrv_port_forceModeAbility_t, 1);
    *pSpeed = fm_ability.speed;
    *pDuplex = fm_ability.duplex;
    *pFlowControl = fm_ability.flowctrl;

    return RT_ERR_OK;
}

int32 rtrpc_port_phyForceModeAbility_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed, rtk_port_duplex_t duplex, rtk_enable_t flowControl)
{
    rtdrv_port_forceModeAbility_t fm_ability;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&fm_ability, 0, sizeof(rtdrv_port_forceModeAbility_t));
    fm_ability.unit = unit;
    fm_ability.port = port;
    fm_ability.speed = speed;
    fm_ability.duplex = duplex;
    fm_ability.flowctrl = flowControl;
    SETSOCKOPT(RTDRV_PORT_FORCE_MODE_ABIL_SET, &fm_ability, rtdrv_port_forceModeAbility_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_port_phyForceFlowctrlMode_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_port_flowctrl_mode_t    *pMode)
{
    rtdrv_port_flowctrl_t   fc;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&fc, 0, sizeof(rtdrv_port_flowctrl_t));
    fc.unit         = unit;
    fc.port         = port;
    GETSOCKOPT(RTDRV_PORT_FORCE_MODE_FLOW_CTRL_MODE_GET, &fc, rtdrv_port_flowctrl_t, 1);
    pMode->tx_pause = fc.tx_status;
    pMode->rx_pause = fc.rx_status;

    return RT_ERR_OK;
}

int32
rtrpc_port_phyForceFlowctrlMode_set(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_port_flowctrl_mode_t    *pMode)
{
    rtdrv_port_flowctrl_t   fc;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&fc, 0, sizeof(rtdrv_port_flowctrl_t));
    fc.unit         = unit;
    fc.port         = port;
    fc.tx_status    = pMode->tx_pause;
    fc.rx_status    = pMode->rx_pause;
    SETSOCKOPT(RTDRV_PORT_FORCE_MODE_FLOW_CTRL_MODE_SET, &fc, rtdrv_port_flowctrl_t, 1);

    return RT_ERR_OK;
}



int32
rtrpc_port_phyMasterSlave_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlaveCfg,
    rtk_port_masterSlave_t   *pMasterSlaveActual)
{
    rtdrv_port_masterSlave_t masterSlave_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&masterSlave_cfg, 0, sizeof(rtdrv_port_masterSlave_t));
    masterSlave_cfg.unit = unit;
    masterSlave_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_MASTER_SLAVE_GET, &masterSlave_cfg, rtdrv_port_masterSlave_t, 1);
    *pMasterSlaveCfg = masterSlave_cfg.masterSlaveCfg;
    *pMasterSlaveActual = masterSlave_cfg.masterSlaveActual;

    return RT_ERR_OK;
}

int32
rtrpc_port_phyMasterSlave_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlaveCfg)
{
    rtdrv_port_masterSlave_t masterSlave_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&masterSlave_cfg, 0, sizeof(rtdrv_port_masterSlave_t));
    masterSlave_cfg.unit = unit;
    masterSlave_cfg.port = port;
    masterSlave_cfg.masterSlaveCfg = masterSlaveCfg;
    SETSOCKOPT(RTDRV_PORT_MASTER_SLAVE_SET, &masterSlave_cfg, rtdrv_port_masterSlave_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_port_phyReg_get(uint32 unit, rtk_port_t port, uint32 page, rtk_port_phy_reg_t reg, uint32 *pData)
{
    rtdrv_port_phyReg_t phy_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&phy_data, 0, sizeof(rtdrv_port_phyReg_t));
    phy_data.unit = unit;
    phy_data.port = port;
    phy_data.page = page;
    phy_data.reg = reg;
    GETSOCKOPT(RTDRV_PORT_PHY_REG_GET, &phy_data, rtdrv_port_phyReg_t, 1);
    *pData = phy_data.data;

    return RT_ERR_OK;
}

int32 rtrpc_port_phyReg_set(uint32 unit, rtk_port_t port, uint32 page, rtk_port_phy_reg_t reg, uint32 data)
{
    rtdrv_port_phyReg_t phy_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&phy_data, 0, sizeof(rtdrv_port_phyReg_t));
    phy_data.unit = unit;
    phy_data.port = port;
    phy_data.page = page;
    phy_data.reg = reg;
    phy_data.data = data;
    SETSOCKOPT(RTDRV_PORT_PHY_REG_SET, &phy_data, rtdrv_port_phyReg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_port_phyExtParkPageReg_get(
    uint32              unit,
    rtk_port_t          port,
    uint32              mainPage,
    uint32              extPage,
    uint32              parkPage,
    rtk_port_phy_reg_t  reg,
    uint32              *pData)
{
    rtdrv_port_phyReg_t phy_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&phy_data, 0, sizeof(rtdrv_port_phyReg_t));
    phy_data.unit = unit;
    phy_data.port = port;
    phy_data.page = mainPage;
    phy_data.extPage = extPage;
    phy_data.parkPage = parkPage;
    phy_data.reg = reg;
    GETSOCKOPT(RTDRV_PORT_PHY_EXT_PARK_PAGE_REG_GET, &phy_data, rtdrv_port_phyReg_t, 1);
    *pData = phy_data.data;

    return RT_ERR_OK;
}    /* end of rtk_port_phyExtParkPageReg_get */

int32
rtrpc_port_phyExtParkPageReg_set(
    uint32              unit,
    rtk_port_t          port,
    uint32              mainPage,
    uint32              extPage,
    uint32              parkPage,
    rtk_port_phy_reg_t  reg,
    uint32              data)
{
    rtdrv_port_phyReg_t phy_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&phy_data, 0, sizeof(rtdrv_port_phyReg_t));
    phy_data.unit = unit;
    phy_data.port = port;
    phy_data.page = mainPage;
    phy_data.extPage = extPage;
    phy_data.parkPage = parkPage;
    phy_data.reg = reg;
    phy_data.data = data;
    SETSOCKOPT(RTDRV_PORT_PHY_EXT_PARK_PAGE_REG_SET, &phy_data, rtdrv_port_phyReg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_port_phyExtParkPageReg_set */

int32
rtrpc_port_phymaskExtParkPageReg_set(
    uint32              unit,
    rtk_portmask_t      *pPortmask,
    uint32              mainPage,
    uint32              extPage,
    uint32              parkPage,
    rtk_port_phy_reg_t  reg,
    uint32              data)
{
    rtdrv_port_phyReg_t phy_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&phy_data, 0, sizeof(rtdrv_port_phyReg_t));
    phy_data.unit = unit;
    phy_data.portmask = *pPortmask;
    phy_data.page = mainPage;
    phy_data.extPage = extPage;
    phy_data.parkPage = parkPage;
    phy_data.reg = reg;
    phy_data.data = data;
    SETSOCKOPT(RTDRV_PORT_PHYMASK_EXT_PARK_PAGE_REG_SET, &phy_data, rtdrv_port_phyReg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_port_phymaskExtParkPageReg_set */

int32
rtrpc_port_phyMmdReg_get(
    uint32              unit,
    rtk_port_t          port,
    uint32              mmdAddr,
    uint32              mmdReg,
    uint32              *pData)
{
    rtdrv_port_phyReg_t phy_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&phy_data, 0, sizeof(rtdrv_port_phyReg_t));
    phy_data.unit = unit;
    phy_data.port = port;
    phy_data.mmdAddr= mmdAddr;
    phy_data.reg = mmdReg;
    GETSOCKOPT(RTDRV_PORT_PHY_MMD_REG_GET, &phy_data, rtdrv_port_phyReg_t, 1);
    *pData = phy_data.data;

    return RT_ERR_OK;
}    /* end of rtk_port_phyMmdReg_get */

int32
rtrpc_port_phyMmdReg_set(
    uint32              unit,
    rtk_port_t          port,
    uint32              mmdAddr,
    uint32              mmdReg,
    uint32              data)
{
    rtdrv_port_phyReg_t phy_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&phy_data, 0, sizeof(rtdrv_port_phyReg_t));
    phy_data.unit = unit;
    phy_data.port = port;
    phy_data.mmdAddr= mmdAddr;
    phy_data.reg = mmdReg;
    phy_data.data = data;
    SETSOCKOPT(RTDRV_PORT_PHY_MMD_REG_SET, &phy_data, rtdrv_port_phyReg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_port_phyMmdReg_set */

int32
rtrpc_port_phymaskMmdReg_set(
    uint32              unit,
    rtk_portmask_t      *pPortmask,
    uint32              mmdAddr,
    uint32              mmdReg,
    uint32              data)
{
    rtdrv_port_phyReg_t phy_data;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&phy_data, 0, sizeof(rtdrv_port_phyReg_t));
    phy_data.unit = unit;
    phy_data.portmask = *pPortmask;
    phy_data.mmdAddr= mmdAddr;
    phy_data.reg = mmdReg;
    phy_data.data = data;
    SETSOCKOPT(RTDRV_PORT_PHYMASK_MMD_REG_SET, &phy_data, rtdrv_port_phyReg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_port_phymaskMmdReg_set */

int32 rtrpc_port_isolation_get(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_ISOLATION_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pPortmask, &port_cfg.portmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}

int32 rtrpc_port_isolation_set(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    osal_memcpy(&port_cfg.portmask, pPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_PORT_ISOLATION_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_port_isolationExt_get(uint32 unit, uint32 devID, rtk_port_t srcPort, rtk_portmask_t *pPortmask)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.devID = devID;
    port_cfg.port = srcPort;
    GETSOCKOPT(RTDRV_PORT_ISOLATIONEXT_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pPortmask, &port_cfg.portmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}

int32 rtrpc_port_isolationExt_set(uint32 unit, uint32 devID, rtk_port_t srcPort, rtk_portmask_t *pPortmask)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.devID = devID;
    port_cfg.port = srcPort;
    osal_memcpy(&port_cfg.portmask, pPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_PORT_ISOLATIONEXT_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_port_isolation_add(uint32 unit, rtk_port_t port, rtk_port_t iso_port)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.targetPort = iso_port;
    SETSOCKOPT(RTDRV_PORT_ISOLATION_ADD, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_port_isolation_add */


int32 rtrpc_port_isolation_del(uint32 unit, rtk_port_t port, rtk_port_t iso_port)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.targetPort = iso_port;
    SETSOCKOPT(RTDRV_PORT_ISOLATION_DEL, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_port_isolation_del */

int32 rtrpc_port_isolationRestrictRoute_get(uint32 unit, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    GETSOCKOPT(RTDRV_PORT_ISOLATION_RESTRICT_ROUTE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pEnable = port_cfg.data;

    return RT_ERR_OK;
} /* end of rtk_port_isolation_del */

int32 rtrpc_port_isolationRestrictRoute_set(uint32 unit, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.data = enable;
    SETSOCKOPT(RTDRV_PORT_ISOLATION_RESTRICT_ROUTE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_port_isolation_del */

int32 rtrpc_port_adminEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnabled)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit=unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit,&unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_EN_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pEnabled = port_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_port_adminEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enabled)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = enabled;
    SETSOCKOPT(RTDRV_PORT_EN_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_port_txEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnabled)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_TX_EN_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pEnabled = port_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_port_txEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enabled)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = enabled;
    SETSOCKOPT(RTDRV_PORT_TX_EN_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_port_rxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnabled)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_RX_EN_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pEnabled = port_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_port_rxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enabled)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = enabled;
    SETSOCKOPT(RTDRV_PORT_RX_EN_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_port_backpressureEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnabled)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_BACK_PRESSURE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pEnabled = port_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_port_backpressureEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enabled)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = enabled;
    SETSOCKOPT(RTDRV_PORT_BACK_PRESSURE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_port_phyComboPortMedia_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_PHY_MEDIA_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pMedia = port_cfg.media;

    return RT_ERR_OK;
}

int32 rtrpc_port_phyComboPortMedia_set(uint32 unit, rtk_port_t port, rtk_port_media_t media)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.media = media;
    SETSOCKOPT(RTDRV_PORT_PHY_MEDIA_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}


int32 rtrpc_port_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_GREEN_ENABLE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pEnable = port_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_port_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = enable;
    SETSOCKOPT(RTDRV_PORT_GREEN_ENABLE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_port_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_GIGA_LITE_ENABLE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pEnable = port_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_port_gigaLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = enable;
    SETSOCKOPT(RTDRV_PORT_GIGA_LITE_ENABLE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}


int32 rtrpc_port_2pt5gLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_2PT5G_LITE_ENABLE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pEnable = port_cfg.data;

    return RT_ERR_OK;
}

int32 rtrpc_port_2pt5gLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = enable;
    SETSOCKOPT(RTDRV_PORT_2PT5G_LITE_ENABLE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_port_phyCrossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    rtdrv_portCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_portCfg_t));
    config.unit = unit;
    config.port = port;
    GETSOCKOPT(RTDRV_PORT_PHY_CROSSOVERMODE_GET, &config, rtdrv_portCfg_t, 1);
    *pMode = config.data;

    return RT_ERR_OK;
} /* end of rtk_port_phyCrossOverMode_get */

int32
rtrpc_port_phyCrossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    rtdrv_portCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_portCfg_t));
    config.unit = unit;
    config.port = port;
    config.data = mode;
    SETSOCKOPT(RTDRV_PORT_PHY_CROSSOVERMODE_SET, &config, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_port_phyCrossOverMode_set */

int32
rtrpc_port_phyCrossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus)
{
    rtdrv_portCfg_t config;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&config, 0, sizeof(rtdrv_portCfg_t));
    config.unit = unit;
    config.port = port;
    GETSOCKOPT(RTDRV_PORT_PHY_CROSSOVERSTATUS_GET, &config, rtdrv_portCfg_t, 1);
    *pStatus = config.data;

    return RT_ERR_OK;
} /* end of rtk_port_phyCrossOverStatus_get */


int32 rtrpc_port_phyComboPortFiberMedia_get(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t *pMedia)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_PHY_FIBER_MEDIA_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pMedia = port_cfg.fiber_media;

    return RT_ERR_OK;
} /* end of rtk_port_phyComboPortFiberMedia_get */

int32 rtrpc_port_phyComboPortFiberMedia_set(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t media)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.fiber_media = media;
    SETSOCKOPT(RTDRV_PORT_PHY_FIBER_MEDIA_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_port_phyComboPortFiberMedia_set */

int32 rtrpc_port_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_LINKDOWN_POWERSAVING_ENABLE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pEnable = port_cfg.data;

    return RT_ERR_OK;
} /* end of rtk_port_linkDownPowerSavingEnable_get */

int32 rtrpc_port_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = enable;
    SETSOCKOPT(RTDRV_PORT_LINKDOWN_POWERSAVING_ENABLE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_port_linkDownPowerSavingEnable_set */

int32 rtrpc_port_vlanBasedIsolationEntry_get(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.index = index;
    GETSOCKOPT(RTDRV_PORT_VLAN_ISOLATION_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEntry, &(port_cfg.vlanIsoEntry), sizeof(rtk_port_vlanIsolationEntry_t));

    return RT_ERR_OK;
}

int32 rtrpc_port_vlanBasedIsolationEntry_set(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.index = index;
    osal_memcpy(&(port_cfg.vlanIsoEntry), pEntry, sizeof(rtk_port_vlanIsolationEntry_t));
    SETSOCKOPT(RTDRV_PORT_VLAN_ISOLATION_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32 rtrpc_port_vlanBasedIsolation_vlanSource_get(uint32 unit, rtk_port_vlanIsolationSrc_t *pVlanSrc)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    GETSOCKOPT(RTDRV_PORT_VLAN_ISOLATION_VLANSOURCE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pVlanSrc = port_cfg.vlanIsoSrc;

    return RT_ERR_OK;
}

int32 rtrpc_port_vlanBasedIsolation_vlanSource_set(uint32 unit, rtk_port_vlanIsolationSrc_t vlanSrc)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.vlanIsoSrc = vlanSrc;
    SETSOCKOPT(RTDRV_PORT_VLAN_ISOLATION_VLANSOURCE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_port_vlanBasedIsolationEgrBypass_get(uint32 unit, rtk_port_t port,rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_VLAN_ISOLATION_EGRBYPASS_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pEnable = port_cfg.data;

    return RT_ERR_OK;
}

int32
rtrpc_port_vlanBasedIsolationEgrBypass_set(uint32 unit, rtk_port_t port,rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = enable;
    SETSOCKOPT(RTDRV_PORT_VLAN_ISOLATION_EGRBYPASS_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_port_fiberDownSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_FIBERDOWNSPEEDENABLE_GET, &cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &cfg.data, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_fiberDownSpeedEnable_get */

int32
rtrpc_port_fiberDownSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.data, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_FIBERDOWNSPEEDENABLE_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_fiberDownSpeedEnable_set */

int32
rtrpc_port_downSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_DOWNSPEEDENABLE_GET, &cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &cfg.data, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_fiberDownSpeedEnable_get */

int32
rtrpc_port_downSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.data, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_DOWNSPEEDENABLE_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_fiberDownSpeedEnable_set */


int32
rtrpc_port_fiberNwayForceLinkEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_FIBERNWAYFORCELINKENABLE_GET, &cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &cfg.data, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_fiberNwayForceLinkEnable_get */

int32
rtrpc_port_fiberNwayForceLinkEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.data, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_FIBERNWAYFORCELINKENABLE_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_fiberNwayForceLinkEnable_set */

/* Function Name:
 *      rtk_port_fiberUnidirEnable_get
 * Description:
 *      Get fiber unidirection enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pEnable       - - pointer to the enable status of mac local loopback
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
rtrpc_port_fiberUnidirEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_FIBERUNIDIRENABLE_GET, &cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_fiberUnidirEnable_get */

/* Function Name:
 *      rtk_port_fiberUnidirEnable_set
 * Description:
 *      Set fiber unidirection enable status of the specific port
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      enable        - enable status of mac local loopback
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
rtrpc_port_fiberUnidirEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_FIBERUNIDIRENABLE_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_fiberUnidirEnable_set */

int32
rtrpc_port_phyLoopBackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_PHYLOOPBACKENABLE_GET, &cfg, rtdrv_portCfg_t, 1);

    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_phyLoopBackEnable_set */

int32
rtrpc_port_phyLoopBackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.data, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_PHYLOOPBACKENABLE_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_phyLoopBackEnable_set */


int32
rtrpc_port_fiberOAMLoopBackEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.data, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_FIBEROAMLOOPBACKENABLE_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_fiberOAMLoopBackEnable_set */

int32
rtrpc_port_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.media_10g, &media, sizeof(rtk_port_10gMedia_t));
    SETSOCKOPT(RTDRV_PORT_10GMEDIA_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_10gMedia_set */

int32
rtrpc_port_10gMedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *media)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == media), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_10GMEDIA_GET, &cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(media, &cfg.media_10g, sizeof(rtk_port_10gMedia_t));

    return RT_ERR_OK;
}   /* end of rtk_port_10gMedia_get */

int32
rtrpc_port_phyFiberTxDis_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_PHYFIBERTXDIS_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_phyFiberTxDis_set */

int32
rtrpc_port_phyFiberTxDisPin_set(uint32 unit, rtk_port_t port, uint32 data)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.data, &data, sizeof(uint32));
    SETSOCKOPT(RTDRV_PORT_PHYFIBERTXDISPIN_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_phyFiberTxDis_set */

int32
rtrpc_port_fiberRxEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_FIBERRXENABLE_GET, &cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_fiberRxEnable_get */

int32
rtrpc_port_fiberRxEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_FIBERRXENABLE_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_fiberRxEnable_set */


int32
rtrpc_port_phyIeeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.testMode, pTestMode, sizeof(rtk_port_phyTestMode_t));
    SETSOCKOPT(RTDRV_PORT_PHYIEEETESTMODE_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}


int32
rtrpc_port_phyPolar_get(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.polarCtrl, pPolarCtrl, sizeof(rtk_port_phyPolarCtrl_t));
    GETSOCKOPT(RTDRV_PORT_PHYPOLAR_GET, &cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pPolarCtrl, &cfg.polarCtrl, sizeof(rtk_port_phyPolarCtrl_t));

    return RT_ERR_OK;
}


int32
rtrpc_port_phyPolar_set(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.polarCtrl, pPolarCtrl, sizeof(rtk_port_phyPolarCtrl_t));
    SETSOCKOPT(RTDRV_PORT_PHYPOLAR_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_port_phyEyeMonitor_start(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 frameNum)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(int32));
    osal_memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&cfg.sdsId, &sdsId, sizeof(uint32));
    osal_memcpy(&cfg.data, &frameNum, sizeof(uint32));
    SETSOCKOPT(RTDRV_PORT_PHYEYEMONITOR_START, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_phyEyeMonitor_start */

int32
rtrpc_port_imageFlash_load(uint32 unit, rtk_port_t port, uint32 size, uint8 *image)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(int32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    memcpy(&cfg.image_size, &size, sizeof(uint32));
    memcpy(&cfg.image, image, sizeof(uint8) * RTK_PORT_FLASHIMG_SIZE);
    SETSOCKOPT(RTDRV_PORT_IMAGEFLASH_LOAD, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_port_phySds_get(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == sdsCfg), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    memcpy(&cfg.sdsCfg, sdsCfg, sizeof(rtk_sdsCfg_t));
    GETSOCKOPT(RTDRV_PORT_PHYSDS_GET, &cfg, rtdrv_portCfg_t, 1);
    memcpy(sdsCfg, &cfg.sdsCfg, sizeof(rtk_sdsCfg_t));

    return RT_ERR_OK;
}   /* end of rtk_port_phySds_get */

int32
rtrpc_port_phySds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == sdsCfg), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    memcpy(&cfg.sdsCfg, sdsCfg, sizeof(rtk_sdsCfg_t));
    SETSOCKOPT(RTDRV_PORT_PHYSDS_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_phySds_set */


int32
rtrpc_port_phySdsRxCaliStatus_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_port_phySdsRxCaliStatus_t *pStatus)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    memcpy(&cfg.data, &sdsId, sizeof(uint32));
    GETSOCKOPT(RTDRV_PORT_PHYSDSRXCALISTATUS_GET, &cfg, rtdrv_portCfg_t, 1);
    memcpy(pStatus, &cfg.phySdsRxCaliStatus, sizeof(rtk_port_phySdsRxCaliStatus_t));

    return RT_ERR_OK;
}

int32
rtrpc_port_phyReset_set(uint32 unit, rtk_port_t port)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    SETSOCKOPT(RTDRV_PORT_PHYRESET_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_port_phyReset_set */

int32
rtrpc_port_phyLinkStatus_get(uint32 unit, rtk_port_t port,
    rtk_port_linkStatus_t *pStatus)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_portCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_PHYLINKSTATUS_GET, &cfg, rtdrv_portCfg_t, 1);
    memcpy(pStatus, &cfg.data, sizeof(rtk_port_linkStatus_t));

    return RT_ERR_OK;
}   /* end of rtrpc_port_phyLinkStatus_get */

int32
rtrpc_port_phyPeerAutoNegoAbility_get(uint32 unit, rtk_port_t port,
    rtk_port_phy_ability_t *pAbility)
{
    rtdrv_port_autoNegoAbility_t an_ability;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&an_ability, 0, sizeof(rtdrv_port_autoNegoAbility_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    memcpy(&an_ability.unit, &unit, sizeof(uint32));
    memcpy(&an_ability.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_PHYPEERAUTONEGOABILITY_GET, &an_ability, rtdrv_port_autoNegoAbility_t, 1);
    memcpy(pAbility, &an_ability.ability, sizeof(rtk_port_phy_ability_t));

    return RT_ERR_OK;
}   /* end of rtrpc_port_phyPeerAutoNegoAbility_get */

int32
rtrpc_port_phyMacIntfSerdesMode_get(uint32 unit, rtk_port_t port,
    rt_serdesMode_t *pSerdesMode)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_portCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pSerdesMode), RT_ERR_NULL_POINTER);

    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_PHYMACINTFSERDESMODE_GET, &cfg, rtdrv_portCfg_t, 1);
    memcpy(pSerdesMode, &cfg.sdsCfg.sdsMode, sizeof(rt_serdesMode_t));

    return RT_ERR_OK;
}   /* end of rtrpc_port_phyMacIntfSerdesMode_get */

int32
rtrpc_port_phyLedMode_set(uint32 unit, rtk_port_t port, rtk_phy_ledMode_t *pLedMode)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    memcpy(&cfg.phyLedMode, pLedMode, sizeof(rtk_phy_ledMode_t));
    SETSOCKOPT(RTDRV_PORT_PHYLEDMODE_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_port_phyLedMode_set */

int32
rtrpc_port_phyLedCtrl_get(uint32 unit, rtk_port_t port,
    rtk_phy_ledCtrl_t *pLedCtrl)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    memcpy(&cfg.phyLedCtrl, pLedCtrl, sizeof(rtk_phy_ledCtrl_t));
    GETSOCKOPT(RTDRV_PORT_PHYLEDCTRL_GET, &cfg, rtdrv_portCfg_t, 1);
    memcpy(pLedCtrl, &cfg.phyLedCtrl, sizeof(rtk_phy_ledCtrl_t));

    return RT_ERR_OK;
}   /* end of rtrpc_port_phyLedCtrl_get */


int32
rtrpc_port_phyLedCtrl_set(uint32 unit, rtk_port_t port,
    rtk_phy_ledCtrl_t *pLedCtrl)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    memcpy(&cfg.phyLedCtrl, pLedCtrl, sizeof(rtk_phy_ledCtrl_t));
    SETSOCKOPT(RTDRV_PORT_PHYLEDCTRL_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_port_phyLedCtrl_set */

int32
rtrpc_port_phyMacIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port,
    rtk_phy_macIntfSdsLinkStatus_t *pStatus)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_portCfg_t));

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_PHYMACINTFSERDESLINKSTATUS_GET, &cfg, rtdrv_portCfg_t, 1);
    memcpy(pStatus, &cfg.status, sizeof(rtk_phy_macIntfSdsLinkStatus_t));

    return RT_ERR_OK;
}   /* end of rtrpc_port_phyMacIntfSerdesLinkStatus_get */


int32
rtrpc_port_phySdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    memcpy(&cfg.sdsId, &sdsId, sizeof(uint32));
    memcpy(&cfg.eyeParam, pEyeParam, sizeof(rtk_sds_eyeParam_t));
    GETSOCKOPT(RTDRV_PORT_PHYEYEPARAM_GET, &cfg, rtdrv_portCfg_t, 1);
    memcpy(pEyeParam, &cfg.eyeParam, sizeof(rtk_sds_eyeParam_t));

    return RT_ERR_OK;
}   /* end of rtrpc_port_phyLedCtrl_set */


int32
rtrpc_port_phySdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam)
{
    rtdrv_portCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    memset(&cfg, 0, sizeof(rtdrv_portCfg_t));
    memcpy(&cfg.unit, &unit, sizeof(uint32));
    memcpy(&cfg.port, &port, sizeof(rtk_port_t));
    memcpy(&cfg.sdsId, &sdsId, sizeof(uint32));
    memcpy(&cfg.eyeParam, pEyeParam, sizeof(rtk_sds_eyeParam_t));
    SETSOCKOPT(RTDRV_PORT_PHYEYEPARAM_SET, &cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtrpc_port_phyLedCtrl_set */


int32
rtrpc_port_phyMdiLoopbackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_MDI_LOOPBACK_ENABLE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pEnable = port_cfg.enable;

    return RT_ERR_OK;
}

int32
rtrpc_port_phyMdiLoopbackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.enable = enable;
    SETSOCKOPT(RTDRV_PORT_MDI_LOOPBACK_ENABLE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_port_specialCongest_get(uint32 unit, rtk_port_t port, uint32* pSecond)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    GETSOCKOPT(RTDRV_PORT_SPECL_CGST_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pSecond = port_cfg.data;
    return RT_ERR_OK;
}

int32
rtrpc_port_specialCongest_set(uint32 unit, rtk_port_t port, uint32 second)
{
    rtdrv_portCfg_t port_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    osal_memset(&port_cfg, 0, sizeof(rtdrv_portCfg_t));
    port_cfg.unit = unit;
    port_cfg.port = port;
    port_cfg.data = second;
    SETSOCKOPT(RTDRV_PORT_SPECL_CGST_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}






int32
rtrpc_portMapper_init(dal_mapper_t *pMapper)
{
    pMapper->port_link_get = rtrpc_port_link_get;
    pMapper->port_linkMedia_get = rtrpc_port_linkMedia_get;
    pMapper->port_speedDuplex_get = rtrpc_port_speedDuplex_get;
    pMapper->port_flowctrl_get = rtrpc_port_flowctrl_get;
    pMapper->port_phyAutoNegoEnable_get = rtrpc_port_phyAutoNegoEnable_get;
    pMapper->port_phyAutoNegoEnable_set = rtrpc_port_phyAutoNegoEnable_set;
    pMapper->port_phyAutoNegoAbility_get = rtrpc_port_phyAutoNegoAbility_get;
    pMapper->port_phyAutoNegoAbility_set = rtrpc_port_phyAutoNegoAbility_set;
    pMapper->port_phyForceModeAbility_get = rtrpc_port_phyForceModeAbility_get;
    pMapper->port_phyForceModeAbility_set = rtrpc_port_phyForceModeAbility_set;
    pMapper->port_phyForceFlowctrlMode_get = rtrpc_port_phyForceFlowctrlMode_get;
    pMapper->port_phyForceFlowctrlMode_set = rtrpc_port_phyForceFlowctrlMode_set;
    pMapper->port_phyMasterSlave_get = rtrpc_port_phyMasterSlave_get;
    pMapper->port_phyMasterSlave_set = rtrpc_port_phyMasterSlave_set;
    pMapper->port_phyReg_get = rtrpc_port_phyReg_get;
    pMapper->port_phyReg_set = rtrpc_port_phyReg_set;
    pMapper->port_phyExtParkPageReg_get = rtrpc_port_phyExtParkPageReg_get;
    pMapper->port_phyExtParkPageReg_set = rtrpc_port_phyExtParkPageReg_set;
    pMapper->port_phymaskExtParkPageReg_set = rtrpc_port_phymaskExtParkPageReg_set;
    pMapper->port_phyMmdReg_get = rtrpc_port_phyMmdReg_get;
    pMapper->port_phyMmdReg_set = rtrpc_port_phyMmdReg_set;
    pMapper->port_phymaskMmdReg_set = rtrpc_port_phymaskMmdReg_set;
    pMapper->port_cpuPortId_get = rtrpc_port_cpuPortId_get;
    pMapper->port_isolation_get = rtrpc_port_isolation_get;
    pMapper->port_isolation_set = rtrpc_port_isolation_set;
    pMapper->port_isolationExt_get = rtrpc_port_isolationExt_get;
    pMapper->port_isolationExt_set = rtrpc_port_isolationExt_set;
    pMapper->port_isolation_add = rtrpc_port_isolation_add;
    pMapper->port_isolation_del = rtrpc_port_isolation_del;
    pMapper->port_isolationRestrictRoute_get = rtrpc_port_isolationRestrictRoute_get;
    pMapper->port_isolationRestrictRoute_set = rtrpc_port_isolationRestrictRoute_set;
    pMapper->port_phyComboPortMedia_get = rtrpc_port_phyComboPortMedia_get;
    pMapper->port_phyComboPortMedia_set = rtrpc_port_phyComboPortMedia_set;
    pMapper->port_backpressureEnable_get = rtrpc_port_backpressureEnable_get;
    pMapper->port_backpressureEnable_set = rtrpc_port_backpressureEnable_set;
    pMapper->port_adminEnable_get = rtrpc_port_adminEnable_get;
    pMapper->port_adminEnable_set = rtrpc_port_adminEnable_set;
    pMapper->port_txEnable_get = rtrpc_port_txEnable_get;
    pMapper->port_txEnable_set = rtrpc_port_txEnable_set;
    pMapper->port_rxEnable_get = rtrpc_port_rxEnable_get;
    pMapper->port_rxEnable_set = rtrpc_port_rxEnable_set;
    pMapper->port_greenEnable_get = rtrpc_port_greenEnable_get;
    pMapper->port_greenEnable_set = rtrpc_port_greenEnable_set;
    pMapper->port_phyCrossOverMode_get = rtrpc_port_phyCrossOverMode_get;
    pMapper->port_phyCrossOverMode_set = rtrpc_port_phyCrossOverMode_set;
    pMapper->port_phyCrossOverStatus_get = rtrpc_port_phyCrossOverStatus_get;
    pMapper->port_phyComboPortFiberMedia_get = rtrpc_port_phyComboPortFiberMedia_get;
    pMapper->port_phyComboPortFiberMedia_set = rtrpc_port_phyComboPortFiberMedia_set;
    pMapper->port_linkDownPowerSavingEnable_get = rtrpc_port_linkDownPowerSavingEnable_get;
    pMapper->port_linkDownPowerSavingEnable_set = rtrpc_port_linkDownPowerSavingEnable_set;
    pMapper->port_vlanBasedIsolationEntry_get = rtrpc_port_vlanBasedIsolationEntry_get;
    pMapper->port_vlanBasedIsolationEntry_set = rtrpc_port_vlanBasedIsolationEntry_set;
    pMapper->port_vlanBasedIsolation_vlanSource_get = rtrpc_port_vlanBasedIsolation_vlanSource_get;
    pMapper->port_vlanBasedIsolation_vlanSource_set = rtrpc_port_vlanBasedIsolation_vlanSource_set;
    pMapper->port_vlanBasedIsolationEgrBypass_get = rtrpc_port_vlanBasedIsolationEgrBypass_get;
    pMapper->port_vlanBasedIsolationEgrBypass_set = rtrpc_port_vlanBasedIsolationEgrBypass_set;
    pMapper->port_downSpeedEnable_get = rtrpc_port_downSpeedEnable_get;
    pMapper->port_downSpeedEnable_set = rtrpc_port_downSpeedEnable_set;
    pMapper->port_fiberDownSpeedEnable_get = rtrpc_port_fiberDownSpeedEnable_get;
    pMapper->port_fiberDownSpeedEnable_set = rtrpc_port_fiberDownSpeedEnable_set;
    pMapper->port_fiberNwayForceLinkEnable_get = rtrpc_port_fiberNwayForceLinkEnable_get;
    pMapper->port_fiberNwayForceLinkEnable_set = rtrpc_port_fiberNwayForceLinkEnable_set;
    pMapper->port_fiberUnidirEnable_get = rtrpc_port_fiberUnidirEnable_get;
    pMapper->port_fiberUnidirEnable_set = rtrpc_port_fiberUnidirEnable_set;
    pMapper->port_fiberOAMLoopBackEnable_set = rtrpc_port_fiberOAMLoopBackEnable_set;
    pMapper->port_phyLoopBackEnable_get = rtrpc_port_phyLoopBackEnable_get;
    pMapper->port_phyLoopBackEnable_set = rtrpc_port_phyLoopBackEnable_set;
    pMapper->port_10gMedia_set = rtrpc_port_10gMedia_set;
    pMapper->port_10gMedia_get = rtrpc_port_10gMedia_get;
    pMapper->port_phyFiberTxDis_set = rtrpc_port_phyFiberTxDis_set;
    pMapper->port_phyFiberTxDisPin_set = rtrpc_port_phyFiberTxDisPin_set;
    pMapper->port_fiberRxEnable_get = rtrpc_port_fiberRxEnable_get;
    pMapper->port_fiberRxEnable_set = rtrpc_port_fiberRxEnable_set;
    pMapper->port_phyIeeeTestMode_set = rtrpc_port_phyIeeeTestMode_set;
    pMapper->port_phyPolar_get = rtrpc_port_phyPolar_get;
    pMapper->port_phyPolar_set = rtrpc_port_phyPolar_set;
    pMapper->port_phyEyeMonitor_start = rtrpc_port_phyEyeMonitor_start;
    pMapper->port_imageFlash_load = rtrpc_port_imageFlash_load;
    pMapper->port_phySds_get = rtrpc_port_phySds_get;
    pMapper->port_phySds_set = rtrpc_port_phySds_set;
    pMapper->port_phySdsRxCaliStatus_get = rtrpc_port_phySdsRxCaliStatus_get;
    pMapper->port_phyReset_set = rtrpc_port_phyReset_set;
    pMapper->port_phyLinkStatus_get = rtrpc_port_phyLinkStatus_get;
    pMapper->port_phyPeerAutoNegoAbility_get = rtrpc_port_phyPeerAutoNegoAbility_get;
    pMapper->port_phyMacIntfSerdesMode_get = rtrpc_port_phyMacIntfSerdesMode_get;
    pMapper->port_phyLedMode_set = rtrpc_port_phyLedMode_set;
    pMapper->port_phyLedCtrl_set = rtrpc_port_phyLedCtrl_set;
    pMapper->port_phyMacIntfSerdesLinkStatus_get = rtrpc_port_phyMacIntfSerdesLinkStatus_get;
    pMapper->port_phySdsEyeParam_get = rtrpc_port_phySdsEyeParam_get;
    pMapper->port_phySdsEyeParam_set = rtrpc_port_phySdsEyeParam_set;
    pMapper->port_phyMdiLoopbackEnable_get = rtrpc_port_phyMdiLoopbackEnable_get;
    pMapper->port_phyMdiLoopbackEnable_set = rtrpc_port_phyMdiLoopbackEnable_set;
    pMapper->port_specialCongest_get = rtrpc_port_specialCongest_get;
    pMapper->port_specialCongest_set = rtrpc_port_specialCongest_set;
    return RT_ERR_OK;
}
