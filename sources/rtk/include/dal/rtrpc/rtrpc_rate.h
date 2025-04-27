/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_RATE_H__
#define __RTRPC_RATE_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_rate_init                                    rtrpc_rate_init
#define rtk_rate_igrBandwidthCtrlIncludeIfg_get          rtrpc_rate_igrBandwidthCtrlIncludeIfg_get
#define rtk_rate_igrBandwidthCtrlIncludeIfg_set          rtrpc_rate_igrBandwidthCtrlIncludeIfg_set
#define rtk_rate_egrBandwidthCtrlIncludeIfg_get          rtrpc_rate_egrBandwidthCtrlIncludeIfg_get
#define rtk_rate_egrBandwidthCtrlIncludeIfg_set          rtrpc_rate_egrBandwidthCtrlIncludeIfg_set
#define rtk_rate_stormControlIncludeIfg_get              rtrpc_rate_stormControlIncludeIfg_get
#define rtk_rate_stormControlIncludeIfg_set              rtrpc_rate_stormControlIncludeIfg_set
#define rtk_rate_includeIfg_get                          rtrpc_rate_includeIfg_get
#define rtk_rate_includeIfg_set                          rtrpc_rate_includeIfg_set
#define rtk_rate_portIgrBwCtrlEnable_get                 rtrpc_rate_portIgrBwCtrlEnable_get
#define rtk_rate_portIgrBwCtrlEnable_set                 rtrpc_rate_portIgrBwCtrlEnable_set
#define rtk_rate_portIgrBwCtrlRate_get                   rtrpc_rate_portIgrBwCtrlRate_get
#define rtk_rate_portIgrBwCtrlRate_set                   rtrpc_rate_portIgrBwCtrlRate_set
#define rtk_rate_igrBandwidthLowThresh_get               rtrpc_rate_igrBandwidthLowThresh_get
#define rtk_rate_igrBandwidthLowThresh_set               rtrpc_rate_igrBandwidthLowThresh_set
#define rtk_rate_portIgrBandwidthHighThresh_get          rtrpc_rate_portIgrBandwidthHighThresh_get
#define rtk_rate_portIgrBandwidthHighThresh_set          rtrpc_rate_portIgrBandwidthHighThresh_set
#define rtk_rate_igrBwCtrlBurstSize_get                  rtrpc_rate_igrBwCtrlBurstSize_get
#define rtk_rate_igrBwCtrlBurstSize_set                  rtrpc_rate_igrBwCtrlBurstSize_set
#define rtk_rate_igrPortBwCtrlBurstSize_get              rtrpc_rate_igrPortBwCtrlBurstSize_get
#define rtk_rate_igrPortBwCtrlBurstSize_set              rtrpc_rate_igrPortBwCtrlBurstSize_set
#define rtk_rate_portIgrBwCtrlBurstSize_get              rtrpc_rate_portIgrBwCtrlBurstSize_get
#define rtk_rate_portIgrBwCtrlBurstSize_set              rtrpc_rate_portIgrBwCtrlBurstSize_set
#define rtk_rate_portIgrBandwidthCtrlExceed_get          rtrpc_rate_portIgrBandwidthCtrlExceed_get
#define rtk_rate_portIgrBandwidthCtrlExceed_reset        rtrpc_rate_portIgrBandwidthCtrlExceed_reset
#define rtk_rate_igrBandwidthCtrlBypass_get              rtrpc_rate_igrBandwidthCtrlBypass_get
#define rtk_rate_igrBandwidthCtrlBypass_set              rtrpc_rate_igrBandwidthCtrlBypass_set
#define rtk_rate_portIgrBwFlowctrlEnable_get             rtrpc_rate_portIgrBwFlowctrlEnable_get
#define rtk_rate_portIgrBwFlowctrlEnable_set             rtrpc_rate_portIgrBwFlowctrlEnable_set
#define rtk_rate_portIgrQueueBwCtrlEnable_get            rtrpc_rate_portIgrQueueBwCtrlEnable_get
#define rtk_rate_portIgrQueueBwCtrlEnable_set            rtrpc_rate_portIgrQueueBwCtrlEnable_set
#define rtk_rate_portIgrQueueBwCtrlRate_get              rtrpc_rate_portIgrQueueBwCtrlRate_get
#define rtk_rate_portIgrQueueBwCtrlRate_set              rtrpc_rate_portIgrQueueBwCtrlRate_set
#define rtk_rate_igrQueueBwCtrlBurstSize_get             rtrpc_rate_igrQueueBwCtrlBurstSize_get
#define rtk_rate_igrQueueBwCtrlBurstSize_set             rtrpc_rate_igrQueueBwCtrlBurstSize_set
#define rtk_rate_portIgrQueueBwCtrlBurstSize_get         rtrpc_rate_portIgrQueueBwCtrlBurstSize_get
#define rtk_rate_portIgrQueueBwCtrlBurstSize_set         rtrpc_rate_portIgrQueueBwCtrlBurstSize_set
#define rtk_rate_portIgrQueueBwCtrlExceed_get            rtrpc_rate_portIgrQueueBwCtrlExceed_get
#define rtk_rate_portIgrQueueBwCtrlExceed_reset          rtrpc_rate_portIgrQueueBwCtrlExceed_reset
#define rtk_rate_portEgrBwCtrlEnable_get                 rtrpc_rate_portEgrBwCtrlEnable_get
#define rtk_rate_portEgrBwCtrlEnable_set                 rtrpc_rate_portEgrBwCtrlEnable_set
#define rtk_rate_portEgrBwCtrlRate_get                   rtrpc_rate_portEgrBwCtrlRate_get
#define rtk_rate_portEgrBwCtrlRate_set                   rtrpc_rate_portEgrBwCtrlRate_set
#define rtk_rate_portEgrBwCtrlBurstSize_get              rtrpc_rate_portEgrBwCtrlBurstSize_get
#define rtk_rate_portEgrBwCtrlBurstSize_set              rtrpc_rate_portEgrBwCtrlBurstSize_set
#define rtk_rate_cpuEgrBandwidthCtrlRateMode_get         rtrpc_rate_cpuEgrBandwidthCtrlRateMode_get
#define rtk_rate_cpuEgrBandwidthCtrlRateMode_set         rtrpc_rate_cpuEgrBandwidthCtrlRateMode_set
#define rtk_rate_egrPortBwCtrlBurstSize_get              rtrpc_rate_egrPortBwCtrlBurstSize_get
#define rtk_rate_egrPortBwCtrlBurstSize_set              rtrpc_rate_egrPortBwCtrlBurstSize_set
#define rtk_rate_portEgrQueueBwCtrlEnable_get            rtrpc_rate_portEgrQueueBwCtrlEnable_get
#define rtk_rate_portEgrQueueBwCtrlEnable_set            rtrpc_rate_portEgrQueueBwCtrlEnable_set
#define rtk_rate_portEgrQueueBwCtrlRate_get              rtrpc_rate_portEgrQueueBwCtrlRate_get
#define rtk_rate_portEgrQueueBwCtrlRate_set              rtrpc_rate_portEgrQueueBwCtrlRate_set
#define rtk_rate_portEgrQueueBwCtrlBurstSize_get         rtrpc_rate_portEgrQueueBwCtrlBurstSize_get
#define rtk_rate_portEgrQueueBwCtrlBurstSize_set         rtrpc_rate_portEgrQueueBwCtrlBurstSize_set
#define rtk_rate_portEgrQueueAssuredBwCtrlEnable_get     rtrpc_rate_portEgrQueueAssuredBwCtrlEnable_get
#define rtk_rate_portEgrQueueAssuredBwCtrlEnable_set     rtrpc_rate_portEgrQueueAssuredBwCtrlEnable_set
#define rtk_rate_portEgrQueueAssuredBwCtrlRate_get       rtrpc_rate_portEgrQueueAssuredBwCtrlRate_get
#define rtk_rate_portEgrQueueAssuredBwCtrlRate_set       rtrpc_rate_portEgrQueueAssuredBwCtrlRate_set
#define rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_get  rtrpc_rate_portEgrQueueAssuredBwCtrlBurstSize_get
#define rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_set  rtrpc_rate_portEgrQueueAssuredBwCtrlBurstSize_set
#define rtk_rate_portEgrQueueAssuredBwCtrlMode_get       rtrpc_rate_portEgrQueueAssuredBwCtrlMode_get
#define rtk_rate_portEgrQueueAssuredBwCtrlMode_set       rtrpc_rate_portEgrQueueAssuredBwCtrlMode_set
#define rtk_rate_egrQueueFixedBandwidthEnable_get        rtrpc_rate_egrQueueFixedBandwidthEnable_get
#define rtk_rate_egrQueueFixedBandwidthEnable_set        rtrpc_rate_egrQueueFixedBandwidthEnable_set
#define rtk_rate_egrQueueBwCtrlBurstSize_get             rtrpc_rate_egrQueueBwCtrlBurstSize_get
#define rtk_rate_egrQueueBwCtrlBurstSize_set             rtrpc_rate_egrQueueBwCtrlBurstSize_set
#define rtk_rate_portStormCtrlEnable_get                 rtrpc_rate_portStormCtrlEnable_get
#define rtk_rate_portStormCtrlEnable_set                 rtrpc_rate_portStormCtrlEnable_set
#define rtk_rate_portStormCtrlRate_get                   rtrpc_rate_portStormCtrlRate_get
#define rtk_rate_portStormCtrlRate_set                   rtrpc_rate_portStormCtrlRate_set
#define rtk_rate_stormControlBurstSize_get               rtrpc_rate_stormControlBurstSize_get
#define rtk_rate_stormControlBurstSize_set               rtrpc_rate_stormControlBurstSize_set
#define rtk_rate_portStormCtrlBurstSize_get              rtrpc_rate_portStormCtrlBurstSize_get
#define rtk_rate_portStormCtrlBurstSize_set              rtrpc_rate_portStormCtrlBurstSize_set
#define rtk_rate_portStormCtrlExceed_get                 rtrpc_rate_portStormCtrlExceed_get
#define rtk_rate_portStormCtrlExceed_reset               rtrpc_rate_portStormCtrlExceed_reset
#define rtk_rate_stormControlRateMode_get                rtrpc_rate_stormControlRateMode_get
#define rtk_rate_stormControlRateMode_set                rtrpc_rate_stormControlRateMode_set
#define rtk_rate_portStormCtrlRateMode_get               rtrpc_rate_portStormCtrlRateMode_get
#define rtk_rate_portStormCtrlRateMode_set               rtrpc_rate_portStormCtrlRateMode_set
#define rtk_rate_portStormCtrlTypeSel_get                rtrpc_rate_portStormCtrlTypeSel_get
#define rtk_rate_portStormCtrlTypeSel_set                rtrpc_rate_portStormCtrlTypeSel_set
#define rtk_rate_stormControlBypass_get                  rtrpc_rate_stormControlBypass_get
#define rtk_rate_stormControlBypass_set                  rtrpc_rate_stormControlBypass_set
#define rtk_rate_portStormCtrlProtoEnable_get            rtrpc_rate_portStormCtrlProtoEnable_get
#define rtk_rate_portStormCtrlProtoEnable_set            rtrpc_rate_portStormCtrlProtoEnable_set
#define rtk_rate_portStormCtrlProtoRate_get              rtrpc_rate_portStormCtrlProtoRate_get
#define rtk_rate_portStormCtrlProtoRate_set              rtrpc_rate_portStormCtrlProtoRate_set
#define rtk_rate_portStormCtrlProtoBurstSize_get         rtrpc_rate_portStormCtrlProtoBurstSize_get
#define rtk_rate_portStormCtrlProtoBurstSize_set         rtrpc_rate_portStormCtrlProtoBurstSize_set
#define rtk_rate_portStormCtrlProtoExceed_get            rtrpc_rate_portStormCtrlProtoExceed_get
#define rtk_rate_portStormCtrlProtoExceed_reset          rtrpc_rate_portStormCtrlProtoExceed_reset
#define rtk_rate_stormCtrlProtoVlanConstrtEnable_get     rtrpc_rate_stormCtrlProtoVlanConstrtEnable_get
#define rtk_rate_stormCtrlProtoVlanConstrtEnable_set     rtrpc_rate_stormCtrlProtoVlanConstrtEnable_set
/*
 * Function Declaration
 */

extern int32
rtrpc_rate_init(uint32 unit);

/* Module Name    : Rate                                            */
/* Sub-module Name: Configuration of ingress port bandwidth control */

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
extern int32
rtrpc_rate_igrBandwidthCtrlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include);

extern int32
rtrpc_rate_igrBandwidthCtrlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include);

extern int32
rtrpc_rate_egrBandwidthCtrlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include);

extern int32
rtrpc_rate_egrBandwidthCtrlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include);

extern int32
rtrpc_rate_stormControlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include);

extern int32
rtrpc_rate_stormControlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include);
#endif

extern int32
rtrpc_rate_includeIfg_get(uint32 unit, rtk_rate_module_t module, rtk_enable_t *pIfg_include);

extern int32
rtrpc_rate_includeIfg_set(uint32 unit, rtk_rate_module_t module, rtk_enable_t ifg_include);

extern int32
rtrpc_rate_portIgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_rate_portIgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_rate_portIgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate);

extern int32
rtrpc_rate_portIgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate);

extern int32
rtrpc_rate_igrBandwidthLowThresh_get(uint32 unit, uint32 *pLowThresh);

extern int32
rtrpc_rate_igrBandwidthLowThresh_set(uint32 unit, uint32 lowThresh);

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
extern int32
rtrpc_rate_portIgrBandwidthHighThresh_get(uint32 unit, rtk_port_t port, uint32 *pHighThresh);

extern int32
rtrpc_rate_portIgrBandwidthHighThresh_set(uint32 unit, rtk_port_t port, uint32 highThresh);
#endif

extern int32
rtrpc_rate_igrBwCtrlBurstSize_get(uint32 unit, uint32 *pBurst_size);

extern int32
rtrpc_rate_igrBwCtrlBurstSize_set(uint32 unit, uint32 burst_size);

extern int32
rtrpc_rate_igrPortBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size);

extern int32
rtrpc_rate_igrPortBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size);

extern int32
rtrpc_rate_portIgrBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t *pCfg);

extern int32
rtrpc_rate_portIgrBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t  *pCfg);

extern int32
rtrpc_rate_portIgrBandwidthCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  *pIsExceed);

extern int32
rtrpc_rate_portIgrBandwidthCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port);

extern int32
rtrpc_rate_igrBandwidthCtrlBypass_get(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t  *pEnable);

extern int32
rtrpc_rate_igrBandwidthCtrlBypass_set(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t enable);

extern int32
rtrpc_rate_portIgrBwFlowctrlEnable_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        *pEnable);

extern int32
rtrpc_rate_portIgrBwFlowctrlEnable_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        enable);

extern int32
rtrpc_rate_portIgrQueueBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable);

extern int32
rtrpc_rate_portIgrQueueBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable);

extern int32
rtrpc_rate_portIgrQueueBwCtrlRate_get(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pRate);

extern int32
rtrpc_rate_portIgrQueueBwCtrlRate_set(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      rate);

extern int32
rtrpc_rate_igrQueueBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size);

extern int32
rtrpc_rate_igrQueueBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size);

extern int32
rtrpc_rate_portIgrQueueBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size);

extern int32
rtrpc_rate_portIgrQueueBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size);

extern int32
rtrpc_rate_portIgrQueueBwCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qid_t               queue,
    uint32                  *pIsExceed);

extern int32
rtrpc_rate_portIgrQueueBwCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qid_t               queue);

/* Module Name    : Rate                                           */
/* Sub-module Name: Configuration of egress port bandwidth control */

extern int32
rtrpc_rate_portEgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_rate_portEgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_rate_portEgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate);

extern int32
rtrpc_rate_portEgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate);

extern int32
rtrpc_rate_portEgrBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, uint32 *pBurst_size);

extern int32
rtrpc_rate_portEgrBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, uint32 burst_size);

extern int32
rtrpc_rate_cpuEgrBandwidthCtrlRateMode_get(uint32 unit, rtk_rate_rateMode_t *pRate_mode);

extern int32
rtrpc_rate_cpuEgrBandwidthCtrlRateMode_set(uint32 unit, rtk_rate_rateMode_t rate_mode);

extern int32
rtrpc_rate_egrPortBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size);

extern int32
rtrpc_rate_egrPortBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size);

extern int32
rtrpc_rate_portEgrQueueBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable);

extern int32
rtrpc_rate_portEgrQueueBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable);

extern int32
rtrpc_rate_portEgrQueueBwCtrlRate_get(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pRate);

extern int32
rtrpc_rate_portEgrQueueBwCtrlRate_set(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      rate);

extern int32
rtrpc_rate_portEgrQueueBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size);

extern int32
rtrpc_rate_portEgrQueueBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size);

extern int32
rtrpc_rate_portEgrQueueAssuredBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable);

extern int32
rtrpc_rate_portEgrQueueAssuredBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable);

extern int32
rtrpc_rate_portEgrQueueAssuredBwCtrlRate_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pRate);

extern int32
rtrpc_rate_portEgrQueueAssuredBwCtrlRate_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 rate);

extern int32
rtrpc_rate_portEgrQueueAssuredBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size);

extern int32
rtrpc_rate_portEgrQueueAssuredBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size);

extern int32
rtrpc_rate_portEgrQueueAssuredBwCtrlMode_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_rate_assuredMode_t    *pMode);

extern int32
rtrpc_rate_portEgrQueueAssuredBwCtrlMode_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_rate_assuredMode_t    mode);

extern int32
rtrpc_rate_egrQueueFixedBandwidthEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable);

extern int32
rtrpc_rate_egrQueueFixedBandwidthEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable);

extern int32
rtrpc_rate_egrQueueBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size);

extern int32
rtrpc_rate_egrQueueBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size);

/* Module Name    : Rate                           */
/* Sub-module Name: Configuration of storm control */

extern int32
rtrpc_rate_portStormCtrlEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            *pEnable);

extern int32
rtrpc_rate_portStormCtrlEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            enable);

extern int32
rtrpc_rate_portStormCtrlRate_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pRate);

extern int32
rtrpc_rate_portStormCtrlRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  rate);

extern int32
rtrpc_rate_portStormCtrlBurstSize_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pBurst_size);

extern int32
rtrpc_rate_portStormCtrlBurstSize_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  burst_size);

extern int32
rtrpc_rate_portStormCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pIsExceed);

extern int32
rtrpc_rate_portStormCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type);

extern int32
rtrpc_rate_portStormCtrlRateMode_get(
    uint32                      unit,
    rtk_port_t              port,
    rtk_rate_storm_rateMode_t   *pRate_mode);

extern int32
rtrpc_rate_portStormCtrlRateMode_set(
    uint32                      unit,
    rtk_port_t              port,
    rtk_rate_storm_rateMode_t   rate_mode);

extern int32
rtrpc_rate_portStormCtrlTypeSel_get(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type, rtk_rate_storm_sel_t *pStorm_sel);

extern int32
rtrpc_rate_portStormCtrlTypeSel_set(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type, rtk_rate_storm_sel_t storm_sel);

extern int32
rtrpc_rate_portStormCtrlProtoEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    rtk_enable_t            *pEnable);

extern int32
rtrpc_rate_portStormCtrlProtoEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    rtk_enable_t            enable);

int32
rtrpc_rate_portStormCtrlProtoRate_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_rate_storm_proto_group_t    storm_type,
    uint32                          *pRate);

int32
rtrpc_rate_portStormCtrlProtoRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                  rate);

extern int32
rtrpc_rate_portStormCtrlProtoBurstSize_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                 *pBurst_size);

extern int32
rtrpc_rate_portStormCtrlProtoBurstSize_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                 burst_size);

extern int32
rtrpc_rate_portStormCtrlProtoExceed_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_rate_storm_proto_group_t    storm_type,
    uint32                          *pIsExceed);

extern int32
rtrpc_rate_portStormCtrlProtoExceed_reset(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_rate_storm_proto_group_t    storm_type);

extern int32
rtrpc_rate_stormControlBypass_get(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t  *pEnable);

extern int32
rtrpc_rate_stormControlBypass_set(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t enable);

extern int32
rtrpc_rate_stormControlBurstSize_get(
    uint32                  unit,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pBurst_size);

extern int32
rtrpc_rate_stormControlBurstSize_set(
    uint32                  unit,
    rtk_rate_storm_group_t  storm_type,
    uint32                  burst_size);

extern int32
rtrpc_rate_stormControlRateMode_get(
    uint32                      unit,
    rtk_rate_storm_rateMode_t   *pRate_mode);

extern int32
rtrpc_rate_stormControlRateMode_set(
    uint32                      unit,
    rtk_rate_storm_rateMode_t   rate_mode);

extern int32
rtrpc_rate_stormCtrlProtoVlanConstrtEnable_get(uint32 unit, rtk_rate_storm_proto_group_t storm_type, rtk_enable_t *pEnable);

extern int32
rtrpc_rate_stormCtrlProtoVlanConstrtEnable_set(uint32 unit, rtk_rate_storm_proto_group_t storm_type, rtk_enable_t enable);

extern int32
rtrpc_rateMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_RATE_H__ */

