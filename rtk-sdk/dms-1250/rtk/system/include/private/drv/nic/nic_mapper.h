/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Mapper Layer is used to seperate different kind of software or hardware platform
 *
 * Feature : Just dispatch information to Multiplex layer
 *
 */
#ifndef __DRV_NIC_MAPPER_H__
#define __DRV_NIC_MAPPER_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <private/drv/nic/nic_common.h>
#include <drv/nic/nic.h>
#include <common/error.h>

/*
 * Symbol Definition
 */
typedef enum nic_controller_e
{
#if defined(CONFIG_SDK_DRIVER_NIC_R8390)
    NIC_R8390,
#endif
#if defined(CONFIG_SDK_DRIVER_NIC_R8380)
    NIC_R8380,
#endif
#if defined(CONFIG_SDK_DRIVER_NIC_R9300)
    NIC_R9300,
#endif
#if defined(CONFIG_SDK_DRIVER_NIC_R9310)
    NIC_R9310,
#endif
    NIC_CTRL_END
} nic_controller_t;

typedef struct nic_mapper_operation_s
{
    /* NIC */
    int32   (*init)(uint32, drv_nic_initCfg_t *);
    int32   (*pkt_tx)(uint32, drv_nic_pkt_t *, drv_nic_tx_cb_f, void *);
    int32   (*rx_start)(uint32);
    int32   (*rx_stop)(uint32);
    int32   (*rx_register)(uint32 , uint8, drv_nic_rx_cb_f, void *, uint32);
    int32   (*rx_unregister)(uint32 , uint8, drv_nic_rx_cb_f);
    int32   (*pkt_alloc)(uint32 , int32 , uint32 , drv_nic_pkt_t **);
    int32   (*pkt_free)(uint32 , drv_nic_pkt_t *);

    int32   (*ringInfo_get)(uint32 , nic_dir_t , uint32 ** , uint32 *);
    int32   (*cpuPortTxRxEnable_set)(uint32 , rtk_enable_t);
    int32   (*intrMask_get)(uint32 , nic_intr_type_t , uint32 *);
    int32   (*intrMask_set)(uint32 , nic_intr_type_t , uint32 );
    int32   (*intrSts_get)(uint32 , nic_intr_type_t , uint32 *);
    int32   (*intrSts_set)(uint32 , nic_intr_type_t , uint32 );
    int32   (*swNicRst_get)(uint32 , uint32 *);
    int32   (*swNicRst_set)(uint32);
    int32   (*swQueRst_get)(uint32 , uint32 *);
    int32   (*swQueRst_set)(uint32);
    int32   (*cpuL2FloodMask_add)(uint32);
    int32   (*cpuL2FloodMask_remove)(uint32);
    int32   (*cpuForceLinkupEnable_set)(uint32 , rtk_enable_t);
    int32   (*holRingSize_set)(uint32 , uint32 , uint32 );
    int32   (*holRingCnt_get)(uint32 , uint32 , uint32 *);
    int32   (*holRingCnt_set)(uint32 , uint32 , uint32 );
    int32   (*ntfyBaseAddr_get)(uint32 , uint32 *);
    int32   (*ntfyBaseAddr_set)(uint32 , uint32 );
    int32   (*ringBaseAddr_get)(uint32 , nic_dir_t , uint32 , uint32 *);
    int32   (*ringBaseAddr_set)(uint32 , nic_dir_t , uint32 , uint32 );
    int32   (*ringCurAddr_get)(uint32 , nic_dir_t , uint32 , uint32 *);
    int32   (*rxTruncateLength_get)(uint32 , uint32 *);
    int32   (*rxTruncateLength_set)(uint32 , uint32 );
    int32   (*nicEnable_get)(uint32 , nic_dir_t , uint32 *);
    int32   (*nicEnable_set)(uint32 , nic_dir_t , uint32 );
    int32   (*nicTxFetch_set)(uint32 , nic_txRing_t , uint32 );
    int32   (*nicTxBusySts_get)(uint32 , nic_txRing_t , uint32 *);
    int32   (*cpuTagId_get)(uint32 , uint32 *);
    int32   (*cpuTagId_set)(uint32 , uint32 );
    int32   (*supportJumboSize_get)(uint32 , uint32 *);
    int32   (*cpuTagFromRaw_cnvt)(uint32 , uint8 * , drv_nic_pkt_t *);
    int32   (*cpuTagToRaw_cnvt)(uint32 , drv_nic_pkt_t * , uint8 *);
    int32   (*cpuTag_dump)(uint32 , drv_nic_pkt_t *);
    int32   (*rawTag_dump)(uint32 , uint8 *);


    /* DIAG */
    int32   (*dbg_set)(uint32, uint32);
    int32   (*dbg_get)(uint32, uint32 *);
    int32   (*cntr_dump)(uint32);
    int32   (*cntr_clear)(uint32);
    int32   (*ringbuf_dump)(uint32);
    int32   (*pkthdr_mbuf_dump)(uint32, uint32, uint32, uint32, uint32);
    int32   (*rx_status_get)(uint32, uint32 *);
    int32   (*nic_reset)(uint32);

} nic_mapper_operation_t;

#define NIC_CTRL(unit)      nic_ops[nic_if[unit]]
#define NIC_CHK(unit)       (nic_if[unit] != CID_GROUP_NONE)

/*
 * Data Declaration
 */
extern nic_mapper_operation_t nic_ops[NIC_CTRL_END];
extern uint32 nic_if[];


/*
 * Macro Definition
 */

/*
 * Function Declaration
 */


#endif /* __DRV_NIC_MAPPER_H __ */

