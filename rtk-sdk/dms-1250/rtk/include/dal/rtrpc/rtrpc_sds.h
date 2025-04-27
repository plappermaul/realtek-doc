/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_SDS_H__
#define __RTRPC_SDS_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_sds_symErr_get                               rtrpc_sds_symErr_get
#define rtk_sds_symErr_clear                             rtrpc_sds_symErr_clear
#define rtk_sds_linkSts_get                              rtrpc_sds_linkSts_get
#define rtk_sds_testModeCnt_get                          rtrpc_sds_testModeCnt_get
#define rtk_sds_testMode_set                             rtrpc_sds_testMode_set
#define rtk_sds_rx_rst                                   rtrpc_sds_rx_rst
#define rtk_sds_leq_get                                  rtrpc_sds_leq_get
#define rtk_sds_leq_set                                  rtrpc_sds_leq_set
#define rtk_sds_leq_adapt                                rtrpc_sds_leq_adapt
#define rtk_sds_xsgNwayEn_set                            rtrpc_sds_xsgNwayEn_set
#define rtk_sds_cmuBand_get                              rtrpc_sds_cmuBand_get
#define rtk_sds_cmuBand_set                              rtrpc_sds_cmuBand_set
#define rtk_sds_eyeMonitor_start                         rtrpc_sds_eyeMonitor_start
#define rtk_sds_eyeParam_get                             rtrpc_sds_eyeParam_get
#define rtk_sds_eyeParam_set                             rtrpc_sds_eyeParam_set
#define rtk_sds_rxCaliConf_get                           rtrpc_sds_rxCaliConf_get
#define rtk_sds_rxCaliConf_set                           rtrpc_sds_rxCaliConf_set
#define rtk_sds_info_get                                 rtrpc_sds_info_get
#define rtk_sds_eyeMonitorInfo_get                       rtrpc_sds_eyeMonitorInfo_get
#define rtk_sds_loopback_get                             rtrpc_sds_loopback_get
#define rtk_sds_loopback_set                             rtrpc_sds_loopback_set
#define rtk_sds_ctrl_get                                 rtrpc_sds_ctrl_get
#define rtk_sds_ctrl_set                                 rtrpc_sds_ctrl_set

/*
 * Function Declaration
 */

extern int32
rtrpc_sds_symErr_get(uint32 unit, uint32 sds, rtk_sds_symErr_t *info);

extern int32
rtrpc_sds_symErr_clear(uint32 unit, uint32 sds);

extern int32
rtrpc_sds_linkSts_get(uint32 unit, uint32 sds, rtk_sds_linkSts_t *info);

extern int32
rtrpc_sds_testModeCnt_get(uint32 unit, uint32 sds, uint32 *cnt);

extern int32
rtrpc_sds_testMode_set(uint32 unit, uint32 sds, rtk_sds_testMode_t mode);

extern int32
rtrpc_sds_rx_rst(uint32  unit, uint32 sds);

extern int32
rtrpc_sds_leq_adapt(uint32  unit, uint32 sds);

extern int32
rtrpc_sds_leq_get(uint32 unit, uint32 sds, rtk_sds_leq_t *pLeq);

extern int32
rtrpc_sds_leq_set(uint32 unit, uint32 sds, rtk_sds_leq_t *pLeq);

extern int32
rtrpc_sds_xsgNwayEn_set(uint32 unit, uint32 sds, rtk_enable_t en);

extern int32
rtrpc_sds_cmuBand_get(uint32 unit, uint32 sds, uint32 *band);

extern int32
rtrpc_sds_cmuBand_set(uint32 unit, uint32 sds, rtk_enable_t en, uint32 val);

extern int32
rtrpc_sds_eyeMonitor_start(uint32 unit, uint32 sds, uint32 frameNum);

extern int32
rtrpc_sds_eyeParam_get(uint32 unit, uint32 sds, rtk_sds_eyeParam_t  *pParam);

extern int32
rtrpc_sds_eyeParam_set(uint32 unit, uint32 sds, rtk_sds_eyeParam_t  param);

extern int32
rtrpc_sds_rxCaliConf_get(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t *pConf);

extern int32
rtrpc_sds_rxCaliConf_set(uint32 unit, uint32 sds, rtk_sds_rxCaliConf_t conf);

extern int32
rtrpc_sdsMapper_init(dal_mapper_t *pMapper);

extern int32
rtrpc_sds_info_get(uint32 unit, uint32 sds, rtk_sds_info_t *pInfo);

extern int32
rtrpc_sds_eyeMonitorInfo_get(uint32 unit, uint32 sds, uint32 frameNum, rtk_sds_eyeMonInfo_t *pInfo);

extern int32
rtrpc_sds_loopback_get(uint32 unit, uint32 sds, rtk_enable_t *pStatus);

extern int32
rtrpc_sds_loopback_set(uint32 unit, uint32 sds, rtk_enable_t status);

extern int32
rtrpc_sds_ctrl_get(uint32 unit, uint32 sds, rtk_sds_ctrl_t ctrlType,
    uint32 *pValue);

extern int32
rtrpc_sds_ctrl_set(uint32 unit, uint32 sds, rtk_sds_ctrl_t ctrlType,
    uint32 value);

#endif /* __RTRPC_SDS_H__ */
