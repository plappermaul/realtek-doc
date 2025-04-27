/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_SWITCH_H__
#define __RTRPC_SWITCH_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_switch_cpuMaxPktLen_get                      rtrpc_switch_cpuMaxPktLen_get
#define rtk_switch_cpuMaxPktLen_set                      rtrpc_switch_cpuMaxPktLen_set
#define rtk_switch_maxPktLenLinkSpeed_get                rtrpc_switch_maxPktLenLinkSpeed_get
#define rtk_switch_maxPktLenLinkSpeed_set                rtrpc_switch_maxPktLenLinkSpeed_set
#define rtk_switch_maxPktLenTagLenCntIncEnable_get       rtrpc_switch_maxPktLenTagLenCntIncEnable_get
#define rtk_switch_maxPktLenTagLenCntIncEnable_set       rtrpc_switch_maxPktLenTagLenCntIncEnable_set
#define rtk_switch_snapMode_get                          rtrpc_switch_snapMode_get
#define rtk_switch_snapMode_set                          rtrpc_switch_snapMode_set
#define rtk_switch_deviceInfo_get                        rtrpc_switch_deviceInfo_get
#define rtk_switch_deviceCapability_print                rtrpc_switch_deviceCapability_print
#define rtk_switch_chksumFailAction_get                  rtrpc_switch_chksumFailAction_get
#define rtk_switch_chksumFailAction_set                  rtrpc_switch_chksumFailAction_set
#define rtk_switch_recalcCRCEnable_get                   rtrpc_switch_recalcCRCEnable_get
#define rtk_switch_recalcCRCEnable_set                   rtrpc_switch_recalcCRCEnable_set
#define rtk_switch_mgmtMacAddr_get                       rtrpc_switch_mgmtMacAddr_get
#define rtk_switch_mgmtMacAddr_set                       rtrpc_switch_mgmtMacAddr_set
#define rtk_switch_IPv4Addr_get                          rtrpc_switch_IPv4Addr_get
#define rtk_switch_IPv4Addr_set                          rtrpc_switch_IPv4Addr_set
#define rtk_switch_pkt2CpuTypeFormat_get                 rtrpc_switch_pkt2CpuTypeFormat_get
#define rtk_switch_pkt2CpuTypeFormat_set                 rtrpc_switch_pkt2CpuTypeFormat_set
#define rtk_switch_pppoeIpParseEnable_get                rtrpc_switch_pppoeIpParseEnable_get
#define rtk_switch_pppoeIpParseEnable_set                rtrpc_switch_pppoeIpParseEnable_set
#define rtk_switch_cpuPktTruncateEnable_get              rtrpc_switch_cpuPktTruncateEnable_get
#define rtk_switch_cpuPktTruncateEnable_set              rtrpc_switch_cpuPktTruncateEnable_set
#define rtk_switch_cpuPktTruncateLen_get                 rtrpc_switch_cpuPktTruncateLen_get
#define rtk_switch_cpuPktTruncateLen_set                 rtrpc_switch_cpuPktTruncateLen_set
#define rtk_switch_portMaxPktLenLinkSpeed_get            rtrpc_switch_portMaxPktLenLinkSpeed_get
#define rtk_switch_portMaxPktLenLinkSpeed_set            rtrpc_switch_portMaxPktLenLinkSpeed_set
#define rtk_switch_portMaxPktLenTagLenCntIncEnable_get   rtrpc_switch_portMaxPktLenTagLenCntIncEnable_get
#define rtk_switch_portMaxPktLenTagLenCntIncEnable_set   rtrpc_switch_portMaxPktLenTagLenCntIncEnable_set
#define rtk_switch_flexTblFmt_get                        rtrpc_switch_flexTblFmt_get
#define rtk_switch_flexTblFmt_set                        rtrpc_switch_flexTblFmt_set
/*
 * Function Declaration
 */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

extern int32
rtrpc_switch_init(uint32 unit);


extern int32
rtrpc_switch_deviceCapability_print(uint32 unit);

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */


extern int32
rtrpc_switch_deviceInfo_get(uint32 unit, rtk_switch_devInfo_t *pDevInfo);

extern int32
rtrpc_switch_cpuMaxPktLen_get(uint32 unit, rtk_switch_pktDir_t dir, uint32 *pLen);

extern int32
rtrpc_switch_cpuMaxPktLen_set(uint32 unit, rtk_switch_pktDir_t dir, uint32 len);

extern int32
rtrpc_switch_maxPktLenLinkSpeed_get(uint32 unit, rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen);

extern int32
rtrpc_switch_maxPktLenLinkSpeed_set(uint32 unit, rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len);

extern int32
rtrpc_switch_maxPktLenTagLenCntIncEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_switch_maxPktLenTagLenCntIncEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_switch_snapMode_get(uint32 unit, rtk_snapMode_t *pSnapMode);

extern int32
rtrpc_switch_snapMode_set(uint32 unit, rtk_snapMode_t snapMode);

extern int32
rtrpc_switch_chksumFailAction_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_switch_chksum_fail_t    failType,
    rtk_action_t                *pAction);

extern int32
rtrpc_switch_chksumFailAction_set(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_switch_chksum_fail_t    failType,
    rtk_action_t                action);

extern int32
rtrpc_switch_recalcCRCEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_switch_recalcCRCEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */

extern int32
rtrpc_switch_mgmtMacAddr_get(uint32 unit, rtk_mac_t *pMac);

extern int32
rtrpc_switch_mgmtMacAddr_set(uint32 unit, rtk_mac_t *pMac);

extern int32
rtrpc_switch_IPv4Addr_get(uint32 unit, uint32 *pIpAddr);

extern int32
rtrpc_switch_IPv4Addr_set(uint32 unit, uint32 ipAddr);

extern int32
rtrpc_switch_pkt2CpuTypeFormat_get(
    uint32                      unit,
    rtk_switch_pkt2CpuType_t    type,
    rtk_pktFormat_t             *pFormat);

extern int32
rtrpc_switch_pkt2CpuTypeFormat_set(
    uint32                      unit,
    rtk_switch_pkt2CpuType_t    type,
    rtk_pktFormat_t             format);

extern int32
rtrpc_switch_pppoeIpParseEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_switch_pppoeIpParseEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_switch_cpuPktTruncateEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_switch_cpuPktTruncateEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_switch_cpuPktTruncateLen_get(uint32 unit, uint32 *pLen);

extern int32
rtrpc_switch_cpuPktTruncateLen_set(uint32 unit, uint32 len);

extern int32
rtrpc_switch_portMaxPktLenLinkSpeed_get(uint32 unit, rtk_port_t port,
    rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen);

extern int32
rtrpc_switch_portMaxPktLenLinkSpeed_set(uint32 unit, rtk_port_t port,
    rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len);

extern int32
rtrpc_switch_portMaxPktLenTagLenCntIncEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable);

extern int32
rtrpc_switch_portMaxPktLenTagLenCntIncEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable);

extern int32
rtrpc_switch_flexTblFmt_get(uint32 unit, rtk_switch_flexTblFmt_t *pFmt);

extern int32
rtrpc_switch_flexTblFmt_set(uint32 unit, rtk_switch_flexTblFmt_t fmt);

extern int32
rtrpc_switchMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_SWITCH_H__ */

