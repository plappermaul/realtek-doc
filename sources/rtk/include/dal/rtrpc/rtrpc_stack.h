/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_STACK_H__
#define __RTRPC_STACK_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_stack_port_get                               rtrpc_stack_port_get
#define rtk_stack_port_set                               rtrpc_stack_port_set
#define rtk_stack_devId_get                              rtrpc_stack_devId_get
#define rtk_stack_devId_set                              rtrpc_stack_devId_set
#define rtk_stack_masterDevId_get                        rtrpc_stack_masterDevId_get
#define rtk_stack_masterDevId_set                        rtrpc_stack_masterDevId_set
#define rtk_stack_loopGuard_get                          rtrpc_stack_loopGuard_get
#define rtk_stack_loopGuard_set                          rtrpc_stack_loopGuard_set
#define rtk_stack_devPortMap_get                         rtrpc_stack_devPortMap_get
#define rtk_stack_devPortMap_set                         rtrpc_stack_devPortMap_set
#define rtk_stack_nonUcastBlockPort_get                  rtrpc_stack_nonUcastBlockPort_get
#define rtk_stack_nonUcastBlockPort_set                  rtrpc_stack_nonUcastBlockPort_set
#define rtk_stack_rmtIntrTxEnable_get                    rtrpc_stack_rmtIntrTxEnable_get
#define rtk_stack_rmtIntrTxEnable_set                    rtrpc_stack_rmtIntrTxEnable_set
#define rtk_stack_rmtIntrTxTriggerEnable_get             rtrpc_stack_rmtIntrTxTriggerEnable_get
#define rtk_stack_rmtIntrTxTriggerEnable_set             rtrpc_stack_rmtIntrTxTriggerEnable_set
#define rtk_stack_rmtIntrRxSeqCmpMargin_get              rtrpc_stack_rmtIntrRxSeqCmpMargin_get
#define rtk_stack_rmtIntrRxSeqCmpMargin_set              rtrpc_stack_rmtIntrRxSeqCmpMargin_set
#define rtk_stack_rmtIntrRxForceUpdateEnable_get         rtrpc_stack_rmtIntrRxForceUpdateEnable_get
#define rtk_stack_rmtIntrRxForceUpdateEnable_set         rtrpc_stack_rmtIntrRxForceUpdateEnable_set
#define rtk_stack_rmtIntrInfo_get                        rtrpc_stack_rmtIntrInfo_get
#define rtk_stack_shrink_get                             rtrpc_stack_shrink_get
#define rtk_stack_shrink_set                             rtrpc_stack_shrink_set


/*
 * Function Declaration
 */

/* Module Name : STACK */

extern int32
rtrpc_stack_init(uint32 unit);

extern int32
rtrpc_stack_cascadeMode_init(uint32 unit);


/* Module Name    : STACK                    */
/* Sub-module Name: User configuration stack */

extern int32
rtrpc_stack_port_get (uint32 unit, rtk_portmask_t *pStkPorts);

extern int32
rtrpc_stack_port_set (uint32 unit, rtk_portmask_t *pStkPorts);

extern int32
rtrpc_stack_devId_get(uint32 unit, uint32 *pMyDevID);


extern int32
rtrpc_stack_devId_set(uint32 unit, uint32 myDevID);

extern int32
rtrpc_stack_masterDevId_get(uint32 unit, uint32 *pMasterDevID);

extern int32
rtrpc_stack_masterDevId_set(uint32 unit, uint32 masterDevID);

extern int32
rtrpc_stack_loopGuard_get(uint32 unit, rtk_enable_t *pEnable);


extern int32
rtrpc_stack_loopGuard_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_stack_devPortMap_get (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts);


extern int32
rtrpc_stack_devPortMap_set (uint32 unit, uint32 dstDevID, rtk_portmask_t *pStkPorts);

extern int32
rtrpc_stack_nonUcastBlockPort_get (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts);


extern int32
rtrpc_stack_nonUcastBlockPort_set (uint32 unit, uint32 srcDevID, rtk_portmask_t *pBlockStkPorts);

extern int32
rtrpc_stack_rmtIntrTxEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_stack_rmtIntrTxEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_stack_rmtIntrTxTriggerEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_stack_rmtIntrTxTriggerEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_stack_rmtIntrRxSeqCmpMargin_get(uint32 unit, int32 *pMargin);

extern int32
rtrpc_stack_rmtIntrRxSeqCmpMargin_set(uint32 unit, int32 margin);

extern int32
rtrpc_stack_rmtIntrRxForceUpdateEnable_get(uint32 unit, rtk_enable_t *pEnable);

extern int32
rtrpc_stack_rmtIntrRxForceUpdateEnable_set(uint32 unit, rtk_enable_t enable);

extern int32
rtrpc_stack_rmtIntrInfo_get(uint32 unit, rtk_stack_rmtIntrInfo_t *pInfo);

extern int32
rtrpc_stack_shrink_get(uint32 unit, rtk_stack_shrinkCtrlType_t shrinkType, uint32 *pVal);

extern int32
rtrpc_stack_shrink_set(uint32 unit, rtk_stack_shrinkCtrlType_t shrinkType, uint32 val);

extern int32
rtrpc_stackMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_STACK_H__ */

