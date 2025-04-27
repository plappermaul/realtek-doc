/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_TIME_H__
#define __RTRPC_TIME_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_time_portPtpEnable_get                       rtrpc_time_portPtpEnable_get
#define rtk_time_portPtpEnable_set                       rtrpc_time_portPtpEnable_set
#define rtk_time_portPtpRxTimestamp_get                  rtrpc_time_portPtpRxTimestamp_get
#define rtk_time_portPtpTxTimestamp_get                  rtrpc_time_portPtpTxTimestamp_get
#define rtk_time_portRefTime_get                         rtrpc_time_portRefTime_get
#define rtk_time_portRefTime_set                         rtrpc_time_portRefTime_set
#define rtk_time_portRefTimeAdjust_set                   rtrpc_time_portRefTimeAdjust_set
#define rtk_time_portRefTimeEnable_get                   rtrpc_time_portRefTimeEnable_get
#define rtk_time_portRefTimeEnable_set                   rtrpc_time_portRefTimeEnable_set
#define rtk_time_portRefTimeFreq_get                     rtrpc_time_portRefTimeFreq_get
#define rtk_time_portRefTimeFreq_set                     rtrpc_time_portRefTimeFreq_set
#define rtk_time_correctionFieldTransparentValue_get     rtrpc_time_correctionFieldTransparentValue_get
#define rtk_time_portPtpMacAddr_get                     rtrpc_time_portPtpMacAddr_get
#define rtk_time_portPtpMacAddr_set                     rtrpc_time_portPtpMacAddr_set
#define rtk_time_portPtpMacAddrRange_get            rtrpc_time_portPtpMacAddrRange_get
#define rtk_time_portPtpMacAddrRange_set            rtrpc_time_portPtpMacAddrRange_set
#define rtk_time_portPtpVlanTpid_get                     rtrpc_time_portPtpVlanTpid_get
#define rtk_time_portPtpVlanTpid_set                     rtrpc_time_portPtpVlanTpid_set
#define rtk_time_portPtpOper_get                          rtrpc_time_portPtpOper_get
#define rtk_time_portPtpOper_set                          rtrpc_time_portPtpOper_set
#define rtk_time_portPtpLatchTime_get                  rtrpc_time_portPtpLatchTime_get
/*
 * Function Declaration
 */

/* Module Name : TIME */

extern int32
rtrpc_time_init(uint32 unit);

extern int32
rtrpc_time_portPtpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_time_portPtpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_time_portPtpRxTimestamp_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_time_ptpIdentifier_t    identifier,
    rtk_time_timeStamp_t        *pTimeStamp);

extern int32
rtrpc_time_portPtpTxTimestamp_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_time_ptpIdentifier_t    identifier,
    rtk_time_timeStamp_t        *pTimeStamp);

extern int32
rtrpc_time_portPtpTxTimestampCallback_register(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_time_ptpIdentifier_t    identifier,
    rtk_time_ptpTime_cb_f       *fCb);

extern int32
rtrpc_time_portRefTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pTimeStamp);

extern int32
rtrpc_time_portRefTime_set(uint32 unit, rtk_portmask_t portmask, rtk_time_timeStamp_t timeStamp);

extern int32
rtrpc_time_portRefTimeAdjust_set(uint32 unit, rtk_portmask_t portmask, uint32 sign, rtk_time_timeStamp_t timeStamp);

extern int32
rtrpc_time_portRefTimeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_time_portRefTimeEnable_set(uint32 unit, rtk_portmask_t portmask, rtk_enable_t enable);

extern int32
rtrpc_time_portRefTimeFreq_get(uint32 unit, rtk_port_t port, uint32 *pFreq);


extern int32
rtrpc_time_portRefTimeFreq_set(uint32 unit, rtk_portmask_t portmask, uint32 freq);

extern int32
rtrpc_time_correctionFieldTransparentValue_get(uint32 unit, int64 oriCf, rtk_time_timeStamp_t rxTimeStamp, int64 *pTransCf);

extern int32
rtk_time_portPtpMacAddr_get(uint32 unit, rtk_port_t port, rtk_mac_t *pMac);

extern int32
rtk_time_portPtpMacAddr_set(uint32 unit, rtk_port_t port, rtk_mac_t *pMac);

extern int32
rtk_time_portPtpMacAddrRange_get(uint32 unit, rtk_port_t port, uint32 *pRange);

extern int32
rtk_time_portPtpMacAddrRange_set(uint32 unit, rtk_port_t port, uint32 range);

extern int32
rtk_time_portPtpVlanTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 idx, uint32 *pTpid);

extern int32
rtk_time_portPtpVlanTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 idx, uint32 tpid);

extern int32
rtk_time_portPtpOper_get(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg);

extern int32
rtk_time_portPtpOper_set(uint32 unit, rtk_port_t port, rtk_time_operCfg_t *pOperCfg);

extern int32
rtk_time_portPtpLatchTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pLatchTime);

extern int32
rtrpc_timeMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_TIME_H__ */
