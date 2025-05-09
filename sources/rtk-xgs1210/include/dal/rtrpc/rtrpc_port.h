/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_PORT_H__
#define __RTRPC_PORT_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_port_link_get                                rtrpc_port_link_get
#define rtk_port_linkMedia_get                           rtrpc_port_linkMedia_get
#define rtk_port_speedDuplex_get                         rtrpc_port_speedDuplex_get
#define rtk_port_flowctrl_get                            rtrpc_port_flowctrl_get
#define rtk_port_cpuPortId_get                           rtrpc_port_cpuPortId_get
#define rtk_port_phyAutoNegoEnable_get                   rtrpc_port_phyAutoNegoEnable_get
#define rtk_port_phyAutoNegoEnable_set                   rtrpc_port_phyAutoNegoEnable_set
#define rtk_port_phyAutoNegoAbility_get                  rtrpc_port_phyAutoNegoAbility_get
#define rtk_port_phyAutoNegoAbility_set                  rtrpc_port_phyAutoNegoAbility_set
#define rtk_port_phyForceModeAbility_get                 rtrpc_port_phyForceModeAbility_get
#define rtk_port_phyForceModeAbility_set                 rtrpc_port_phyForceModeAbility_set
#define rtk_port_phyForceFlowctrlMode_get                rtrpc_port_phyForceFlowctrlMode_get
#define rtk_port_phyForceFlowctrlMode_set                rtrpc_port_phyForceFlowctrlMode_set
#define rtk_port_phyMasterSlave_get                      rtrpc_port_phyMasterSlave_get
#define rtk_port_phyMasterSlave_set                      rtrpc_port_phyMasterSlave_set
#define rtk_port_phyReg_get                              rtrpc_port_phyReg_get
#define rtk_port_phyReg_set                              rtrpc_port_phyReg_set
#define rtk_port_phyExtParkPageReg_get                   rtrpc_port_phyExtParkPageReg_get
#define rtk_port_phyExtParkPageReg_set                   rtrpc_port_phyExtParkPageReg_set
#define rtk_port_phymaskExtParkPageReg_set               rtrpc_port_phymaskExtParkPageReg_set
#define rtk_port_phyMmdReg_get                           rtrpc_port_phyMmdReg_get
#define rtk_port_phyMmdReg_set                           rtrpc_port_phyMmdReg_set
#define rtk_port_phymaskMmdReg_set                       rtrpc_port_phymaskMmdReg_set
#define rtk_port_isolation_get                           rtrpc_port_isolation_get
#define rtk_port_isolation_set                           rtrpc_port_isolation_set
#define rtk_port_isolationExt_get                        rtrpc_port_isolationExt_get
#define rtk_port_isolationExt_set                        rtrpc_port_isolationExt_set
#define rtk_port_isolation_add                           rtrpc_port_isolation_add
#define rtk_port_isolation_del                           rtrpc_port_isolation_del
#define rtk_port_isolationRestrictRoute_get              rtrpc_port_isolationRestrictRoute_get
#define rtk_port_isolationRestrictRoute_set              rtrpc_port_isolationRestrictRoute_set
#define rtk_port_adminEnable_get                         rtrpc_port_adminEnable_get
#define rtk_port_adminEnable_set                         rtrpc_port_adminEnable_set
#define rtk_port_txEnable_get                            rtrpc_port_txEnable_get
#define rtk_port_txEnable_set                            rtrpc_port_txEnable_set
#define rtk_port_rxEnable_get                            rtrpc_port_rxEnable_get
#define rtk_port_rxEnable_set                            rtrpc_port_rxEnable_set
#define rtk_port_backpressureEnable_get                  rtrpc_port_backpressureEnable_get
#define rtk_port_backpressureEnable_set                  rtrpc_port_backpressureEnable_set
#define rtk_port_phyComboPortMedia_get                   rtrpc_port_phyComboPortMedia_get
#define rtk_port_phyComboPortMedia_set                   rtrpc_port_phyComboPortMedia_set
#define rtk_port_greenEnable_get                         rtrpc_port_greenEnable_get
#define rtk_port_greenEnable_set                         rtrpc_port_greenEnable_set
#define rtk_port_gigaLiteEnable_get                      rtrpc_port_gigaLiteEnable_get
#define rtk_port_gigaLiteEnable_set                      rtrpc_port_gigaLiteEnable_set
#define rtk_port_2pt5gLiteEnable_get                     rtrpc_port_2pt5gLiteEnable_get
#define rtk_port_2pt5gLiteEnable_set                     rtrpc_port_2pt5gLiteEnable_set
#define rtk_port_phyCrossOverMode_get                    rtrpc_port_phyCrossOverMode_get
#define rtk_port_phyCrossOverMode_set                    rtrpc_port_phyCrossOverMode_set
#define rtk_port_phyCrossOverStatus_get                  rtrpc_port_phyCrossOverStatus_get
#define rtk_port_phyComboPortFiberMedia_get              rtrpc_port_phyComboPortFiberMedia_get
#define rtk_port_phyComboPortFiberMedia_set              rtrpc_port_phyComboPortFiberMedia_set
#define rtk_port_linkDownPowerSavingEnable_get           rtrpc_port_linkDownPowerSavingEnable_get
#define rtk_port_linkDownPowerSavingEnable_set           rtrpc_port_linkDownPowerSavingEnable_set
#define rtk_port_vlanBasedIsolationEntry_get             rtrpc_port_vlanBasedIsolationEntry_get
#define rtk_port_vlanBasedIsolationEntry_set             rtrpc_port_vlanBasedIsolationEntry_set
#define rtk_port_vlanBasedIsolation_vlanSource_get       rtrpc_port_vlanBasedIsolation_vlanSource_get
#define rtk_port_vlanBasedIsolation_vlanSource_set       rtrpc_port_vlanBasedIsolation_vlanSource_set
#define rtk_port_vlanBasedIsolationEgrBypass_get         rtrpc_port_vlanBasedIsolationEgrBypass_get
#define rtk_port_vlanBasedIsolationEgrBypass_set         rtrpc_port_vlanBasedIsolationEgrBypass_set
#define rtk_port_fiberDownSpeedEnable_get                rtrpc_port_fiberDownSpeedEnable_get
#define rtk_port_fiberDownSpeedEnable_set                rtrpc_port_fiberDownSpeedEnable_set
#define rtk_port_downSpeedEnable_get                     rtrpc_port_downSpeedEnable_get
#define rtk_port_downSpeedEnable_set                     rtrpc_port_downSpeedEnable_set
#define rtk_port_fiberNwayForceLinkEnable_get            rtrpc_port_fiberNwayForceLinkEnable_get
#define rtk_port_fiberNwayForceLinkEnable_set            rtrpc_port_fiberNwayForceLinkEnable_set
#define rtk_port_fiberUnidirEnable_get                   rtrpc_port_fiberUnidirEnable_get
#define rtk_port_fiberUnidirEnable_set                   rtrpc_port_fiberUnidirEnable_set
#define rtk_port_phyLoopBackEnable_get                   rtrpc_port_phyLoopBackEnable_get
#define rtk_port_phyLoopBackEnable_set                   rtrpc_port_phyLoopBackEnable_set
#define rtk_port_fiberOAMLoopBackEnable_set              rtrpc_port_fiberOAMLoopBackEnable_set
#define rtk_port_10gMedia_set                            rtrpc_port_10gMedia_set
#define rtk_port_10gMedia_get                            rtrpc_port_10gMedia_get
#define rtk_port_phyFiberTxDis_set                       rtrpc_port_phyFiberTxDis_set
#define rtk_port_phyFiberTxDisPin_set                    rtrpc_port_phyFiberTxDisPin_set
#define rtk_port_fiberRxEnable_get                       rtrpc_port_fiberRxEnable_get
#define rtk_port_fiberRxEnable_set                       rtrpc_port_fiberRxEnable_set
#define rtk_port_phyIeeeTestMode_set                     rtrpc_port_phyIeeeTestMode_set
#define rtk_port_phyPolar_get                            rtrpc_port_phyPolar_get
#define rtk_port_phyPolar_set                            rtrpc_port_phyPolar_set
#define rtk_port_phyEyeMonitor_start                     rtrpc_port_phyEyeMonitor_start
#define rtk_port_imageFlash_load                         rtrpc_port_imageFlash_load
#define rtk_port_phySds_get                              rtrpc_port_phySds_get
#define rtk_port_phySds_set                              rtrpc_port_phySds_set
#define rtk_port_phySdsRxCaliStatus_get                  rtrpc_port_phySdsRxCaliStatus_get
#define rtk_port_phyReset_set                            rtrpc_port_phyReset_set
#define rtk_port_phyLinkStatus_get                       rtrpc_port_phyLinkStatus_get
#define rtk_port_phyPeerAutoNegoAbility_get              rtrpc_port_phyPeerAutoNegoAbility_get
#define rtk_port_phyMacIntfSerdesMode_get                rtrpc_port_phyMacIntfSerdesMode_get
#define rtk_port_phyLedMode_set                          rtrpc_port_phyLedMode_set
#define rtk_port_phyLedCtrl_get                          rtrpc_port_phyLedCtrl_get
#define rtk_port_phyLedCtrl_set                          rtrpc_port_phyLedCtrl_set
#define rtk_port_phyMacIntfSerdesLinkStatus_get          rtrpc_port_phyMacIntfSerdesLinkStatus_get
#define rtk_port_phySdsEyeParam_get                      rtrpc_port_phySdsEyeParam_get
#define rtk_port_phySdsEyeParam_set                      rtrpc_port_phySdsEyeParam_set
#define rtk_port_phyMdiLoopbackEnable_get                      rtrpc_port_phyMdiLoopbackEnable_get
#define rtk_port_phyMdiLoopbackEnable_set                      rtrpc_port_phyMdiLoopbackEnable_set
#define rtk_port_specialCongest_get                      rtrpc_port_specialCongest_get
#define rtk_port_specialCongest_set                      rtrpc_port_specialCongest_set

/*
 * Function Declaration
 */

extern int32
rtrpc_port_init(uint32 unit);

/* Module Name    : Port                                       */
/* Sub-module Name: Parameter settings for the port-based view */

extern int32
rtrpc_port_link_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus);

extern int32
rtrpc_port_linkMedia_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus, rtk_port_media_t *pMedia);

extern int32
rtrpc_port_speedDuplex_get(
    uint32            unit,
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex);

extern int32
rtrpc_port_flowctrl_get(
    uint32      unit,
    rtk_port_t  port,
    uint32      *pTxStatus,
    uint32      *pRxStatus);

extern int32
rtrpc_port_phyAutoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_port_phyAutoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_port_phyAutoNegoAbility_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility);

extern int32
rtrpc_port_phyAutoNegoAbility_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility);

extern int32
rtrpc_port_phyForceModeAbility_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_speed_t    *pSpeed,
    rtk_port_duplex_t   *pDuplex,
    rtk_enable_t        *pFlowControl);

extern int32
rtrpc_port_phyForceModeAbility_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_speed_t    speed,
    rtk_port_duplex_t   duplex,
    rtk_enable_t        flowControl);

extern int32
rtrpc_port_phyForceFlowctrlMode_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_port_flowctrl_mode_t    *pMode);

extern int32
rtrpc_port_phyForceFlowctrlMode_set(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_port_flowctrl_mode_t    *pMode);

extern int32
rtrpc_port_phyMasterSlave_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlaveCfg,
    rtk_port_masterSlave_t   *pMasterSlaveActual);

extern int32
rtrpc_port_phyMasterSlave_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_port_masterSlave_t  masterSlave);

extern int32
rtrpc_port_phyReg_get(
    uint32              unit,
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              *pData);

extern int32
rtrpc_port_phyReg_set(
    uint32              unit,
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              data);

extern int32
rtrpc_port_phyExtParkPageReg_get(
    uint32              unit,
    rtk_port_t          port,
    uint32              mainPage,
    uint32              extPage,
    uint32              parkPage,
    rtk_port_phy_reg_t  reg,
    uint32              *pData);

extern int32
rtrpc_port_phyExtParkPageReg_set(
    uint32              unit,
    rtk_port_t          port,
    uint32              mainPage,
    uint32              extPage,
    uint32              parkPage,
    rtk_port_phy_reg_t  reg,
    uint32              data);

extern int32
rtrpc_port_phymaskExtParkPageReg_set(
    uint32              unit,
    rtk_portmask_t      *pPortmask,
    uint32              mainPage,
    uint32              extPage,
    uint32              parkPage,
    rtk_port_phy_reg_t  reg,
    uint32              data);

extern int32
rtrpc_port_phyMmdReg_get(
    uint32              unit,
    rtk_port_t          port,
    uint32              mmdAddr,
    uint32              mmdReg,
    uint32              *pData);

extern int32
rtrpc_port_phyMmdReg_set(
    uint32              unit,
    rtk_port_t          port,
    uint32              mmdAddr,
    uint32              mmdReg,
    uint32              data);

extern int32
rtrpc_port_phymaskMmdReg_set(
    uint32              unit,
    rtk_portmask_t      *pPortmask,
    uint32              mmdAddr,
    uint32              mmdReg,
    uint32              data);

extern int32
rtrpc_port_cpuPortId_get(uint32 unit, rtk_port_t *pPort);

extern int32
rtrpc_port_isolation_get(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask);

extern int32
rtrpc_port_isolation_set(uint32 unit, rtk_port_t port, rtk_portmask_t *pPortmask);

extern int32
rtrpc_port_isolationExt_get(uint32 unit, uint32 devID, rtk_port_t srcPort, rtk_portmask_t *pPortmask);

extern int32
rtrpc_port_isolationExt_set(uint32 unit, uint32 devID, rtk_port_t srcPort, rtk_portmask_t *pPortmask);

extern int32
rtrpc_port_isolation_add(uint32 unit, rtk_port_t port, rtk_port_t iso_port);

extern int32
rtrpc_port_isolation_del(uint32 unit, rtk_port_t port, rtk_port_t iso_port);

extern int32
rtrpc_port_isolationRestrictRoute_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_port_isolationRestrictRoute_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_port_phyComboPortMedia_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia);

extern int32
rtrpc_port_phyComboPortMedia_set(uint32 unit, rtk_port_t port, rtk_port_media_t media);

extern int32
rtrpc_port_backpressureEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_port_backpressureEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_port_adminEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_port_adminEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_port_linkMon_enable(uint32 unit, uint32 scan_interval_us);

extern int32
rtrpc_port_linkMon_disable(uint32 unit);

extern int32
rtrpc_port_linkMon_register(rtk_port_linkMon_callback_t linkMon_callback);

extern int32
rtrpc_port_linkMon_unregister(rtk_port_linkMon_callback_t linkMon_callback);

extern int32
rtrpc_port_linkMon_swScanPorts_set(uint32 unit, rtk_portmask_t *pSwScan_portmask);

extern int32
rtrpc_port_linkMon_swScanPorts_get(uint32 unit, rtk_portmask_t *pSwScan_portmask);

extern int32
rtrpc_port_txEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_port_txEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_port_rxEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_port_rxEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_port_specialCongest_get(uint32 unit, rtk_port_t port, uint32* pSecond);

extern int32
rtrpc_port_specialCongest_set(uint32 unit, rtk_port_t port, uint32 second);

extern int32
rtrpc_port_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_port_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_port_phyCrossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode);

extern int32
rtrpc_port_phyCrossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode);

extern int32
rtrpc_port_phyCrossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus);


extern int32
rtrpc_port_flowCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_port_flowCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_port_phyComboPortFiberMedia_get(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t *pMedia);

extern int32
rtrpc_port_phyComboPortFiberMedia_set(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t media);

extern int32
rtrpc_port_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_port_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_port_vlanBasedIsolationEntry_get(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry);

extern int32
rtrpc_port_vlanBasedIsolationEntry_set(uint32 unit, uint32 index, rtk_port_vlanIsolationEntry_t* pEntry);

extern int32
rtrpc_port_vlanBasedIsolation_vlanSource_get(uint32 unit, rtk_port_vlanIsolationSrc_t *pVlanSrc);

extern int32
rtrpc_port_vlanBasedIsolation_vlanSource_set(uint32 unit, rtk_port_vlanIsolationSrc_t vlanSrc);

extern int32
rtrpc_port_vlanBasedIsolationEgrBypass_get(uint32 unit, rtk_port_t port,rtk_enable_t *pEnable);

extern int32
rtrpc_port_vlanBasedIsolationEgrBypass_set(uint32 unit, rtk_port_t port,rtk_enable_t enable);

extern int32
rtrpc_port_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_port_gigaLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_port_2pt5gLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_port_2pt5gLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_port_phyReconfig_register(uint32 unit, rtk_port_phyReconfig_callback_t phyNotification_callback);

extern int32
rtrpc_port_phyReconfig_unregister(uint32 unit);

extern int32
rtrpc_port_downSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

extern int32
rtrpc_port_downSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

extern int32
rtrpc_port_fiberDownSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

extern int32
rtrpc_port_fiberDownSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

extern int32
rtrpc_port_fiberNwayForceLinkEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

extern int32
rtrpc_port_fiberNwayForceLinkEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

extern int32
rtrpc_port_fiberUnidirEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_port_fiberUnidirEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_port_fiberOAMLoopBackEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

extern int32
rtrpc_port_phyLoopBackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_port_phyLoopBackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_port_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media);

extern int32
rtrpc_port_10gMedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *media);

extern int32
rtrpc_port_phyFiberTxDis_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_port_phyFiberTxDisPin_set(uint32 unit, rtk_port_t port, uint32 data);

extern int32
rtrpc_port_fiberRxEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

extern int32
rtrpc_port_fiberRxEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

extern int32
rtrpc_port_phyIeeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode);


extern int32
rtrpc_port_phyPolar_get(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl);

extern int32
rtrpc_port_phyPolar_set(uint32 unit, rtk_port_t port, rtk_port_phyPolarCtrl_t *pPolarCtrl);

extern int32
rtrpc_port_phyEyeMonitor_start(uint32 unit, rtk_port_t port, uint32 sdsId, uint32 frameNum);

int32
rtrpc_port_imageFlash_load(uint32 unit, rtk_port_t port, uint32 size, uint8 *image);

extern int32
rtrpc_port_phySds_get(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg);

extern int32
rtrpc_port_phySds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg);

extern int32
rtrpc_port_phySdsRxCaliStatus_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_port_phySdsRxCaliStatus_t *pStatus);

extern int32
rtrpc_port_phyReset_set(uint32 unit, rtk_port_t port);

extern int32
rtrpc_port_phyLinkStatus_get(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t *pStatus);

extern int32
rtrpc_port_phyPeerAutoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility);

extern int32
rtrpc_port_phyMacIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pSerdesMode);

extern int32
rtrpc_port_phyLedMode_set(uint32 unit, rtk_port_t port, rtk_phy_ledMode_t *pLedMode);

extern int32
rtrpc_port_phyLedCtrl_get(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl);

extern int32
rtrpc_port_phyLedCtrl_set(uint32 unit, rtk_port_t port, rtk_phy_ledCtrl_t *pLedCtrl);

extern int32
rtrpc_port_phyMacIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port, rtk_phy_macIntfSdsLinkStatus_t *pStatus);

extern int32
rtrpc_port_phySdsEyeParam_get(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);

extern int32
rtrpc_port_phySdsEyeParam_set(uint32 unit, rtk_port_t port, uint32 sdsId, rtk_sds_eyeParam_t *pEyeParam);

extern int32
rtrpc_port_phyMdiLoopbackEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_port_phyMdiLoopbackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


extern int32
rtrpc_portMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_PORT_H__ */
