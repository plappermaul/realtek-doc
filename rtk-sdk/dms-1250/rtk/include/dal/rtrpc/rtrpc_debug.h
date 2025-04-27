/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_DEBUG_H__
#define __RTRPC_DEBUG_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#include <hal/mac/mac_debug.h>
#include <hal/phy/phy_rtl8295_patch.h>

#ifdef RTUSR
#define debug_mem_show                           rtrpc_debug_mem_show
#define hal_dumpHsa                              rtrpc_hal_dumpHsa
#define hal_dumpHsa_openflow                     rtrpc_hal_dumpHsa_openflow
#define hal_dumpHsb                              rtrpc_hal_dumpHsb
#define hal_dumpHsb_openflow                     rtrpc_hal_dumpHsb_openflow
#define hal_dumpHsm                              rtrpc_hal_dumpHsm
#define hal_dumpHsm_openflow                     rtrpc_hal_dumpHsm_openflow
#define hal_dumpHsmIdx                           rtrpc_hal_dumpHsmIdx
#define hal_dumpPmi                              rtrpc_hal_dumpPmi
#define hal_dumpPpi                              rtrpc_hal_dumpPpi
#define hal_getDbgCntr                           rtrpc_hal_getDbgCntr
#define hal_getDbgEncapCntr                      rtrpc_hal_getDbgEncapCntr
#define hal_getFlowCtrlEgrPortUsedPageCnt        rtrpc_hal_getFlowCtrlEgrPortUsedPageCnt
#define hal_getFlowCtrlIgrPortUsedPageCnt        rtrpc_hal_getFlowCtrlIgrPortUsedPageCnt
#define hal_getFlowCtrlPortIgrQueueUsedPageCnt   rtrpc_hal_getFlowCtrlPortIgrQueueUsedPageCnt
#define hal_getFlowCtrlPortQueueUsedPageCnt      rtrpc_hal_getFlowCtrlPortQueueUsedPageCnt
#define hal_getFlowCtrlRepctQueueCntrInfo        rtrpc_hal_getFlowCtrlRepctQueueCntrInfo
#define hal_getFlowCtrlSystemIgrQueueUsedPageCnt rtrpc_hal_getFlowCtrlSystemIgrQueueUsedPageCnt
#define hal_getFlowCtrlSystemPublicUsedPageCnt   rtrpc_hal_getFlowCtrlSystemPublicUsedPageCnt
#define hal_getFlowCtrlSystemPublicResource_get  rtrpc_hal_getFlowCtrlSystemPublicResource_get
#define hal_getFlowCtrlSystemUsedPageCnt         rtrpc_hal_getFlowCtrlSystemUsedPageCnt
#define hal_getWatchdogCnt                       rtrpc_hal_getWatchdogCnt
#define hal_repctQueueEmptyStatus_get            rtrpc_hal_repctQueueEmptyStatus_get
#define hal_repctQueueFetchEnable_get            rtrpc_hal_repctQueueFetchEnable_get
#define hal_repctQueueFetchEnable_set            rtrpc_hal_repctQueueFetchEnable_set
#define hal_repctQueueStickEnable_get            rtrpc_hal_repctQueueStickEnable_get
#define hal_repctQueueStickEnable_set            rtrpc_hal_repctQueueStickEnable_set
#define hal_resetFlowCtrlEgrPortUsedPageCnt      rtrpc_hal_resetFlowCtrlEgrPortUsedPageCnt
#define hal_resetFlowCtrlIgrPortUsedPageCnt      rtrpc_hal_resetFlowCtrlIgrPortUsedPageCnt
#define hal_resetFlowCtrlRepctQueueUsedPageCnt   rtrpc_hal_resetFlowCtrlRepctQueueUsedPageCnt
#define hal_resetFlowCtrlSystemUsedPageCnt       rtrpc_hal_resetFlowCtrlSystemUsedPageCnt
#define hal_rtl8295_reg_read                     rtrpc_hal_rtl8295_reg_read
#define hal_rtl8295_reg_write                    rtrpc_hal_rtl8295_reg_write
#define hal_rtl8295_sds_read                     rtrpc_hal_rtl8295_sds_read
#define hal_rtl8295_sds_write                    rtrpc_hal_rtl8295_sds_write
#define rt_log_config_get                        rtrpc_rt_log_config_get
#define rt_log_enable_get                        rtrpc_rt_log_enable_get
#define rt_log_enable_set                        rtrpc_rt_log_enable_set
#define rt_log_format_get                        rtrpc_rt_log_format_get
#define rt_log_format_set                        rtrpc_rt_log_format_set
#define rt_log_level_get                         rtrpc_rt_log_level_get
#define rt_log_level_set                         rtrpc_rt_log_level_set
#define rt_log_mask_get                          rtrpc_rt_log_mask_get
#define rt_log_mask_set                          rtrpc_rt_log_mask_set
#define rt_log_moduleMask_get                    rtrpc_rt_log_moduleMask_get
#define rt_log_moduleMask_set                    rtrpc_rt_log_moduleMask_set
#define rt_log_type_get                          rtrpc_rt_log_type_get
#define rt_log_type_set                          rtrpc_rt_log_type_set
#endif //RTUSR

#define mac_debug_sds_rxCali                     rtrpc_mac_debug_sds_rxCali
#define mac_debug_sds_rxCali_debugEnable_set     rtrpc_mac_debug_sds_rxCali_debugEnable_set
#define mac_debug_sds_rxCaliConf_get             rtrpc_mac_debug_sds_rxCaliConf_get
#define mac_debug_sds_rxCaliConf_set             rtrpc_mac_debug_sds_rxCaliConf_set
#define mac_debug_sds_rxCaliEnable_get           rtrpc_mac_debug_sds_rxCaliEnable_get
#define mac_debug_sds_rxCaliEnable_set           rtrpc_mac_debug_sds_rxCaliEnable_set
#define mac_debug_sds_rxCaliStatus_get           rtrpc_mac_debug_sds_rxCaliStatus_get
#define phy_8214qf_rxCaliConf_get                rtrpc_phy_8214qf_rxCaliConf_get
#define phy_8214qf_rxCaliConf_set                rtrpc_phy_8214qf_rxCaliConf_set
#define phy_8295_diag_set                        rtrpc_phy_8295_diag_set
#define phy_8295_patch_debugEnable_set           rtrpc_phy_8295_patch_debugEnable_set
#define phy_8295r_rxCaliConfPort_get             rtrpc_phy_8295r_rxCaliConfPort_get
#define phy_8295r_rxCaliConfPort_set             rtrpc_phy_8295r_rxCaliConfPort_set
#define phy_8390_10gMedia_set                    rtrpc_phy_8390_10gMedia_set
#define _dal_phy_debug_cmd                       rtrpc__dal_phy_debug_cmd
#define dal_linkMonPolling_stop                  rtrpc_dal_linkMonPolling_stop

int32 rtrpc_rt_log_enable_get(rtk_enable_t *pEnabled);
int32 rtrpc_rt_log_enable_set(rtk_enable_t enabled);
int32 rtrpc_rt_log_level_get(uint32 *pLv);
int32 rtrpc_rt_log_level_set(uint32 lv);
int32 rtrpc_rt_log_mask_get(uint32 *pMask);
int32 rtrpc_rt_log_mask_set(uint32 mask);
int32 rtrpc_rt_log_type_get(uint32 *pType);
int32 rtrpc_rt_log_type_set(uint32 type);
int32 rtrpc_rt_log_format_get(uint32 *pFormat);
int32 rtrpc_rt_log_format_set(uint32 format);
int32 rtrpc_rt_log_moduleMask_get(uint64 *pMask);
int32 rtrpc_rt_log_moduleMask_set(uint64 mask);
int32 rtrpc_rt_log_config_get(uint32 *pCfg);
uint8** rtrpc_rt_log_moduleName_get(uint64 module);
int32 rtrpc_rt_log(const int32 level, const int64 module, const char *format, ...);
int32 rtrpc_hal_dumpHsb(uint32 unit);
int32 rtrpc_hal_dumpHsb_openflow(uint32 unit);
int32 rtrpc_hal_dumpPpi(uint32 unit, uint32 index);
int32 rtrpc_hal_dumpPmi(uint32 unit);
int32 rtrpc_hal_dumpHsa(uint32 unit);
int32 rtrpc_hal_dumpHsa_openflow(uint32 unit);
int32 rtrpc_hal_dumpHsm(uint32 unit);
int32 rtrpc_hal_dumpHsm_openflow(uint32 unit);
int32 rtrpc_hal_dumpHsmIdx(uint32 unit, uint32 index);
int32 rtrpc_hal_getDbgCntr(uint32 unit, rtk_dbg_mib_dbgType_t type, uint32 *pCntr);
int32 rtrpc_hal_getDbgEncapCntr(uint32 unit, rtk_dbg_encap_cntr_t *pEncapCntr);
int32 rtrpc_hal_getFlowCtrlIgrPortUsedPageCnt(uint32 unit, rtk_port_t port, uint32 *pCntr, uint32 *pMaxCntr);
int32 rtrpc_hal_getFlowCtrlEgrPortUsedPageCnt(uint32 unit, rtk_port_t port, uint32 *pCntr, uint32 *pMaxCntr);
int32 rtrpc_hal_getFlowCtrlSystemUsedPageCnt(uint32 unit, uint32 *pCntr, uint32 *pMaxCntr);
int32 rtrpc_hal_getFlowCtrlSystemPublicUsedPageCnt(uint32 unit, uint32 *pCntr, uint32 *pMaxCntr);
int32 rtrpc_hal_getFlowCtrlSystemPublicResource_get(uint32 unit, uint32 *pCntr);
int32 rtrpc_hal_getFlowCtrlSystemIgrQueueUsedPageCnt(uint32 unit, rtk_dbg_queue_usedPageCnt_t *pQCntr, rtk_dbg_queue_usedPageCnt_t *pQMaxCntr);
int32 rtrpc_hal_getFlowCtrlPortIgrQueueUsedPageCnt(uint32 unit, rtk_port_t port, rtk_dbg_queue_usedPageCnt_t *pQCntr, rtk_dbg_queue_usedPageCnt_t *pQMaxCntr);
int32 rtrpc_hal_getFlowCtrlPortQueueUsedPageCnt(uint32 unit, rtk_port_t port, rtk_dbg_queue_usedPageCnt_t *pQCntr, rtk_dbg_queue_usedPageCnt_t *pQMaxCntr);
int32 rtrpc_hal_getFlowCtrlRepctQueueCntrInfo(uint32 unit, rtk_dbg_repctQ_CntrInfo_t *pRepctCntr);
int32 rtrpc_hal_resetFlowCtrlIgrPortUsedPageCnt(uint32 unit, rtk_port_t port);
int32 rtrpc_hal_resetFlowCtrlEgrPortUsedPageCnt(uint32 unit, rtk_port_t port);
int32 rtrpc_hal_resetFlowCtrlSystemUsedPageCnt(uint32 unit);
int32 rtrpc_hal_resetFlowCtrlRepctQueueUsedPageCnt(uint32 unit);
int32 rtrpc_hal_repctQueueEmptyStatus_get(uint32 unit, rtk_enable_t *pEnable);
int32 rtrpc_hal_repctQueueStickEnable_get(uint32 unit, rtk_enable_t *pEnable);
int32 rtrpc_hal_repctQueueStickEnable_set(uint32 unit, rtk_enable_t enable);
int32 rtrpc_hal_repctQueueFetchEnable_get(uint32 unit, rtk_enable_t *pEnable);
int32 rtrpc_hal_repctQueueFetchEnable_set(uint32 unit, rtk_enable_t enable);
int32 rtrpc_hal_getWatchdogCnt(uint32 unit, uint32 type, uint32 * count);
int32 rtrpc_debug_mem_show(uint32 unit);
#if defined(CONFIG_SDK_RTL8295R) || defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
int32 rtrpc_hal_rtl8295_reg_read(uint32 unit, uint32 port, uint32 reg_addr, uint32 *pData);
int32 rtrpc_hal_rtl8295_reg_write(uint32 unit, rtk_port_t port, uint32 reg_addr, uint32 data);
int32 rtrpc_hal_rtl8295_sds_read(uint32 unit, rtk_port_t port, uint32 sds_idx, uint32 page, uint32 reg, uint32 *pData);
int32 rtrpc_hal_rtl8295_sds_write(uint32 unit, rtk_port_t port, uint32 sds_idx, uint32 page, uint32 reg, uint32 data);
int32 rtrpc_phy_8295_diag_set(uint32 unit, rtk_port_t port, rtk_port_t mdxMacId, uint32 sds, uint8 *name);
int32 rtrpc_phy_8295_patch_debugEnable_set(uint32 enable);
#endif
#if defined(CONFIG_SDK_RTL8295R)
int32 rtrpc_phy_8295r_rxCaliConfPort_set(uint32 unit, rtk_port_t port, phy_8295_rxCaliConf_t *conf);
int32 rtrpc_phy_8295r_rxCaliConfPort_get(uint32 unit, rtk_port_t port, phy_8295_rxCaliConf_t *conf);
#endif
#if defined(CONFIG_SDK_RTL8214QF) || defined(CONFIG_SDK_RTL8224QF)
int32 rtrpc_phy_8214qf_rxCaliConf_set(uint32 unit, rtk_port_t basePort, phy_8295_rxCaliConf_t *conf);
int32 rtrpc_phy_8214qf_rxCaliConf_get(uint32 unit, rtk_port_t basePort, phy_8295_rxCaliConf_t *conf);
#endif
int32 rtrpc_phy_8390_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media);
int32 rtrpc_mac_debug_sds_rxCaliEnable_get(uint32 unit, uint32 sds, uint32 *pEnable);
int32 rtrpc_mac_debug_sds_rxCaliEnable_set(uint32 unit, uint32 sds, rtk_enable_t enable);
int32 rtrpc_mac_debug_sds_rxCaliConf_get(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t *pConf);
int32 rtrpc_mac_debug_sds_rxCaliConf_set(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t conf);
int32 rtrpc_mac_debug_sds_rxCaliStatus_get(uint32 unit, uint32 sds, rtk_port_phySdsRxCaliStatus_t *pStatus);
int32 rtrpc_mac_debug_sds_rxCali(uint32 unit, uint32 sds, uint32 retryCnt);
int32 rtrpc_mac_debug_sds_rxCali_debugEnable_set(rtk_enable_t enable);
int32 rtrpc__dal_phy_debug_cmd(uint32 unit, char *cmd_str, rtk_portmask_t *portmask, uint32 param1, uint32 param2, uint32 param3, uint32 param4, uint32 param5);
int32 rtrpc_dal_linkMonPolling_stop(uint32 unit,uint32 stop_bool);

#endif
