/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_FLOWCTRL_H__
#define __RTRPC_FLOWCTRL_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_flowctrl_init                                rtrpc_flowctrl_init
#define rtk_flowctrl_portPauseOnAction_get               rtrpc_flowctrl_portPauseOnAction_get
#define rtk_flowctrl_portPauseOnAction_set               rtrpc_flowctrl_portPauseOnAction_set
#define rtk_flowctrl_portPauseOnAllowedPageNum_get       rtrpc_flowctrl_portPauseOnAllowedPageNum_get
#define rtk_flowctrl_portPauseOnAllowedPageNum_set       rtrpc_flowctrl_portPauseOnAllowedPageNum_set
#define rtk_flowctrl_pauseOnAllowedPktLen_get            rtrpc_flowctrl_pauseOnAllowedPktLen_get
#define rtk_flowctrl_pauseOnAllowedPktLen_set            rtrpc_flowctrl_pauseOnAllowedPktLen_set
#define rtk_flowctrl_pauseOnAllowedPktNum_get            rtrpc_flowctrl_pauseOnAllowedPktNum_get
#define rtk_flowctrl_pauseOnAllowedPktNum_set            rtrpc_flowctrl_pauseOnAllowedPktNum_set
#define rtk_flowctrl_igrGuarEnable_get                   rtrpc_flowctrl_igrGuarEnable_get
#define rtk_flowctrl_igrGuarEnable_set                   rtrpc_flowctrl_igrGuarEnable_set
#define rtk_flowctrl_igrSystemPauseThresh_get            rtrpc_flowctrl_igrSystemPauseThresh_get
#define rtk_flowctrl_igrSystemPauseThresh_set            rtrpc_flowctrl_igrSystemPauseThresh_set
#define rtk_flowctrl_igrJumboSystemPauseThresh_get       rtrpc_flowctrl_igrJumboSystemPauseThresh_get
#define rtk_flowctrl_igrJumboSystemPauseThresh_set       rtrpc_flowctrl_igrJumboSystemPauseThresh_set
#define rtk_flowctrl_igrPauseThreshGroup_get             rtrpc_flowctrl_igrPauseThreshGroup_get
#define rtk_flowctrl_igrPauseThreshGroup_set             rtrpc_flowctrl_igrPauseThreshGroup_set
#define rtk_flowctrl_portIgrPortThreshGroupSel_get       rtrpc_flowctrl_portIgrPortThreshGroupSel_get
#define rtk_flowctrl_portIgrPortThreshGroupSel_set       rtrpc_flowctrl_portIgrPortThreshGroupSel_set
#define rtk_flowctrl_igrSystemCongestThresh_get          rtrpc_flowctrl_igrSystemCongestThresh_get
#define rtk_flowctrl_igrSystemCongestThresh_set          rtrpc_flowctrl_igrSystemCongestThresh_set
#define rtk_flowctrl_igrJumboSystemCongestThresh_get     rtrpc_flowctrl_igrJumboSystemCongestThresh_get
#define rtk_flowctrl_igrJumboSystemCongestThresh_set     rtrpc_flowctrl_igrJumboSystemCongestThresh_set
#define rtk_flowctrl_igrCongestThreshGroup_get           rtrpc_flowctrl_igrCongestThreshGroup_get
#define rtk_flowctrl_igrCongestThreshGroup_set           rtrpc_flowctrl_igrCongestThreshGroup_set
#define rtk_flowctrl_jumboModeStatus_get                 rtrpc_flowctrl_jumboModeStatus_get
#define rtk_flowctrl_jumboModeEnable_get                 rtrpc_flowctrl_jumboModeEnable_get
#define rtk_flowctrl_jumboModeEnable_set                 rtrpc_flowctrl_jumboModeEnable_set
#define rtk_flowctrl_jumboModeLength_get                 rtrpc_flowctrl_jumboModeLength_get
#define rtk_flowctrl_jumboModeLength_set                 rtrpc_flowctrl_jumboModeLength_set
#define rtk_flowctrl_egrSystemUtilThresh_get             rtrpc_flowctrl_egrSystemUtilThresh_get
#define rtk_flowctrl_egrSystemUtilThresh_set             rtrpc_flowctrl_egrSystemUtilThresh_set
#define rtk_flowctrl_egrSystemDropThresh_get             rtrpc_flowctrl_egrSystemDropThresh_get
#define rtk_flowctrl_egrSystemDropThresh_set             rtrpc_flowctrl_egrSystemDropThresh_set
#define rtk_flowctrl_egrPortDropThresh_get               rtrpc_flowctrl_egrPortDropThresh_get
#define rtk_flowctrl_egrPortDropThresh_set               rtrpc_flowctrl_egrPortDropThresh_set
#define rtk_flowctrl_egrPortDropEnable_get               rtrpc_flowctrl_egrPortDropEnable_get
#define rtk_flowctrl_egrPortDropEnable_set               rtrpc_flowctrl_egrPortDropEnable_set
#define rtk_flowctrl_egrPortQueueDropEnable_get          rtrpc_flowctrl_egrPortQueueDropEnable_get
#define rtk_flowctrl_egrPortQueueDropEnable_set          rtrpc_flowctrl_egrPortQueueDropEnable_set
#define rtk_flowctrl_egrQueueDropThresh_get              rtrpc_flowctrl_egrQueueDropThresh_get
#define rtk_flowctrl_egrQueueDropThresh_set              rtrpc_flowctrl_egrQueueDropThresh_set
#define rtk_flowctrl_egrCpuQueueDropThresh_get           rtrpc_flowctrl_egrCpuQueueDropThresh_get
#define rtk_flowctrl_egrCpuQueueDropThresh_set           rtrpc_flowctrl_egrCpuQueueDropThresh_set
#define rtk_flowctrl_portEgrDropRefCongestEnable_get     rtrpc_flowctrl_portEgrDropRefCongestEnable_get
#define rtk_flowctrl_portEgrDropRefCongestEnable_set     rtrpc_flowctrl_portEgrDropRefCongestEnable_set
#define rtk_flowctrl_egrPortDropThreshGroup_get          rtrpc_flowctrl_egrPortDropThreshGroup_get
#define rtk_flowctrl_egrPortDropThreshGroup_set          rtrpc_flowctrl_egrPortDropThreshGroup_set
#define rtk_flowctrl_egrQueueDropThreshGroup_get         rtrpc_flowctrl_egrQueueDropThreshGroup_get
#define rtk_flowctrl_egrQueueDropThreshGroup_set         rtrpc_flowctrl_egrQueueDropThreshGroup_set
#define rtk_flowctrl_portEgrDropThreshGroupSel_get       rtrpc_flowctrl_portEgrDropThreshGroupSel_get
#define rtk_flowctrl_portEgrDropThreshGroupSel_set       rtrpc_flowctrl_portEgrDropThreshGroupSel_set
#define rtk_flowctrl_egrQueueDropEnable_get              rtrpc_flowctrl_egrQueueDropEnable_get
#define rtk_flowctrl_egrQueueDropEnable_set              rtrpc_flowctrl_egrQueueDropEnable_set
#define rtk_flowctrl_portEgrQueueDropForceEnable_get     rtrpc_flowctrl_portEgrQueueDropForceEnable_get
#define rtk_flowctrl_portEgrQueueDropForceEnable_set     rtrpc_flowctrl_portEgrQueueDropForceEnable_set
#define rtk_flowctrl_igrQueueDropEnable_get              rtrpc_flowctrl_igrQueueDropEnable_get
#define rtk_flowctrl_igrQueueDropEnable_set              rtrpc_flowctrl_igrQueueDropEnable_set
#define rtk_flowctrl_igrQueuePauseDropThreshGroupSel_get rtrpc_flowctrl_igrQueuePauseDropThreshGroupSel_get
#define rtk_flowctrl_igrQueuePauseDropThreshGroupSel_set rtrpc_flowctrl_igrQueuePauseDropThreshGroupSel_set
#define rtk_flowctrl_igrQueueDropThreshGroup_get         rtrpc_flowctrl_igrQueueDropThreshGroup_get
#define rtk_flowctrl_igrQueueDropThreshGroup_set         rtrpc_flowctrl_igrQueueDropThreshGroup_set
#define rtk_flowctrl_igrQueuePauseThreshGroup_get        rtrpc_flowctrl_igrQueuePauseThreshGroup_get
#define rtk_flowctrl_igrQueuePauseThreshGroup_set        rtrpc_flowctrl_igrQueuePauseThreshGroup_set
#define rtk_flowctrl_igrQueueDropThresh_get              rtrpc_flowctrl_igrQueueDropThresh_get
#define rtk_flowctrl_igrQueueDropThresh_set              rtrpc_flowctrl_igrQueueDropThresh_set
#define rtk_flowctrl_portHolTrafficDropEnable_get        rtrpc_flowctrl_portHolTrafficDropEnable_get
#define rtk_flowctrl_portHolTrafficDropEnable_set        rtrpc_flowctrl_portHolTrafficDropEnable_set
#define rtk_flowctrl_holTrafficTypeDropEnable_get        rtrpc_flowctrl_holTrafficTypeDropEnable_get
#define rtk_flowctrl_holTrafficTypeDropEnable_set        rtrpc_flowctrl_holTrafficTypeDropEnable_set
#define rtk_flowctrl_specialCongestThreshold_get         rtrpc_flowctrl_specialCongestThreshold_get
#define rtk_flowctrl_specialCongestThreshold_set         rtrpc_flowctrl_specialCongestThreshold_set
#define rtk_flowctrl_e2eCascadePortThresh_get            rtrpc_flowctrl_e2eCascadePortThresh_get
#define rtk_flowctrl_e2eCascadePortThresh_set            rtrpc_flowctrl_e2eCascadePortThresh_set
#define rtk_flowctrl_e2eRemotePortPauseThreshGroup_get   rtrpc_flowctrl_e2eRemotePortPauseThreshGroup_get
#define rtk_flowctrl_e2eRemotePortPauseThreshGroup_set   rtrpc_flowctrl_e2eRemotePortPauseThreshGroup_set
#define rtk_flowctrl_e2eRemotePortCongestThreshGroup_get rtrpc_flowctrl_e2eRemotePortCongestThreshGroup_get
#define rtk_flowctrl_e2eRemotePortCongestThreshGroup_set rtrpc_flowctrl_e2eRemotePortCongestThreshGroup_set
#define rtk_flowctrl_portE2eRemotePortThreshGroupSel_get rtrpc_flowctrl_portE2eRemotePortThreshGroupSel_get
#define rtk_flowctrl_portE2eRemotePortThreshGroupSel_set rtrpc_flowctrl_portE2eRemotePortThreshGroupSel_set
#define rtk_flowctrl_tagPauseEnable_get                  rtrpc_flowctrl_tagPauseEnable_get
#define rtk_flowctrl_tagPauseEnable_set                  rtrpc_flowctrl_tagPauseEnable_set
#define rtk_flowctrl_halfConsecutiveRetryEnable_get      rtrpc_flowctrl_halfConsecutiveRetryEnable_get
#define rtk_flowctrl_halfConsecutiveRetryEnable_set      rtrpc_flowctrl_halfConsecutiveRetryEnable_set
/*
 * Function Declaration
 */

extern int32
rtrpc_flowctrl_init(uint32 unit);

/* Module Name    : Flow Control               */
/* Sub-module Name: Flow control configuration */

extern int32
rtrpc_flowctrl_portPauseOnAction_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_flowctrl_pauseOnAction_t    *pAction);

extern int32
rtrpc_flowctrl_portPauseOnAction_set(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_flowctrl_pauseOnAction_t    action);

extern int32
rtrpc_flowctrl_portPauseOnAllowedPageNum_get(uint32 unit, rtk_port_t port, uint32 *pPageNum);

extern int32
rtrpc_flowctrl_portPauseOnAllowedPageNum_set(uint32 unit, rtk_port_t port, uint32 pageNum);

extern int32
rtrpc_flowctrl_pauseOnAllowedPktLen_get(uint32 unit, rtk_port_t port, uint32 *pPktLen);

extern int32
rtrpc_flowctrl_pauseOnAllowedPktLen_set(uint32 unit, rtk_port_t port, uint32 pktLen);

extern int32
rtrpc_flowctrl_pauseOnAllowedPktNum_get(uint32 unit, rtk_port_t port, uint32 *pPktNum);

extern int32
rtrpc_flowctrl_pauseOnAllowedPktNum_set(uint32 unit, rtk_port_t port, uint32 pktNum);


/*
 * Flow Control ON
 */

extern int32
rtrpc_flowctrl_igrGuarEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_flowctrl_igrGuarEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_flowctrl_igrSystemPauseThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrSystemPauseThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrJumboSystemPauseThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrJumboSystemPauseThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrPauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrPauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_portIgrPortThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx);

extern int32
rtrpc_flowctrl_portIgrPortThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx);


/* Module Name    : Flow Control              */
/* Sub-module Name: Egress drop configuration */

/*
 * Flow Control OFF
 */

extern int32
rtrpc_flowctrl_igrSystemCongestThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrSystemCongestThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrJumboSystemCongestThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrJumboSystemCongestThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);


extern int32
rtrpc_flowctrl_igrCongestThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrCongestThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_jumboModeStatus_get(uint32 unit, uint32 * pStatus);

extern int32
rtrpc_flowctrl_jumboModeEnable_get(uint32 unit, rtk_enable_t * pEnable);

extern int32
rtrpc_flowctrl_jumboModeEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_flowctrl_jumboModeLength_get(uint32 unit, uint32 * pLength);

extern int32
rtrpc_flowctrl_jumboModeLength_set(uint32 unit, uint32 length);

extern int32
rtrpc_flowctrl_egrSystemUtilThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_egrSystemUtilThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_egrSystemDropThresh_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_egrSystemDropThresh_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_egrPortDropThresh_get(uint32 unit, rtk_port_t port, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_egrPortDropThresh_set(uint32 unit, rtk_port_t port, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_egrPortDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_flowctrl_egrPortDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


extern int32
rtrpc_flowctrl_egrPortQueueDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_flowctrl_egrPortQueueDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_flowctrl_egrQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_egrQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_egrCpuQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_egrCpuQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_portEgrDropRefCongestEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_flowctrl_portEgrDropRefCongestEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_flowctrl_egrPortDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t * pThresh);

extern int32
rtrpc_flowctrl_egrPortDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_egrQueueDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_egrQueueDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_portEgrDropThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx);

extern int32
rtrpc_flowctrl_portEgrDropThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx);

extern int32
rtrpc_flowctrl_egrQueueDropEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable);

extern int32
rtrpc_flowctrl_egrQueueDropEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable);


extern int32
rtrpc_flowctrl_portEgrQueueDropForceEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable);

extern int32
rtrpc_flowctrl_portEgrQueueDropForceEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable);

extern int32
rtrpc_flowctrl_igrQueueDropEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable);

extern int32
rtrpc_flowctrl_igrQueueDropEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable);

extern int32
rtrpc_flowctrl_igrQueuePauseDropThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx);

extern int32
rtrpc_flowctrl_igrQueuePauseDropThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx);

extern int32
rtrpc_flowctrl_igrQueueDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrQueueDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrQueuePauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrQueuePauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_igrQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_portHolTrafficDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

extern int32
rtrpc_flowctrl_portHolTrafficDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

extern int32
rtrpc_flowctrl_holTrafficTypeDropEnable_get(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t *pEnable);

extern int32
rtrpc_flowctrl_holTrafficTypeDropEnable_set(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t enable);

extern int32
rtrpc_flowctrl_specialCongestThreshold_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_specialCongestThreshold_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);


extern int32
rtrpc_flowctrl_e2eCascadePortThresh_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_e2eCascadePortThresh_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_e2eRemotePortPauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_e2eRemotePortPauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_e2eRemotePortCongestThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_e2eRemotePortCongestThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

extern int32
rtrpc_flowctrl_portE2eRemotePortThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx);

extern int32
rtrpc_flowctrl_portE2eRemotePortThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx);

extern int32
rtrpc_flowctrl_tagPauseEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_flowctrl_tagPauseEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_flowctrl_halfConsecutiveRetryEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_flowctrl_halfConsecutiveRetryEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_flowctrlMapper_init(dal_mapper_t *pMapper);

#endif  /* __RTRPC_FLOWCTRL_H__ */

