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
 * Purpose : Definition those public NIC(Network Interface Controller) APIs and
 *           its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) CPU tag
 *           2) NIC tx
 *           3) NIC rx
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <private/drv/nic/nic_mapper.h>
#include <common/debug/rt_log.h>
#include <hwp/hw_profile.h>

#if defined(CONFIG_SDK_DRIVER_NIC_R8390)
    #include <private/drv/nic/nic_rtl8390.h>
#endif

#if defined(CONFIG_SDK_DRIVER_NIC_R8380)
    #include <private/drv/nic/nic_rtl8380.h>
#endif

#if defined(CONFIG_SDK_DRIVER_NIC_R9310)
    #include <private/drv/nic/nic_rtl9310.h>
#endif

#if defined(CONFIG_SDK_DRIVER_NIC_R9300)
    #include <private/drv/nic/nic_rtl9300.h>
#endif


/*
 * Symbol Definition
 */
#define NIC_DB_SIZE     (sizeof(nic_db)/sizeof(cid_prefix_group_t))

/*
 * Data Declaration
 */
const static cid_prefix_group_t nic_db[] =
{
#if defined(CONFIG_SDK_DRIVER_NIC_R8390)
    {RTL8390_FAMILY_ID, NIC_R8390},
    {RTL8350_FAMILY_ID, NIC_R8390},
#endif
#if defined(CONFIG_SDK_DRIVER_NIC_R8380)
    {RTL8380_FAMILY_ID, NIC_R8380},
    {RTL8330_FAMILY_ID, NIC_R8380},
#endif
#if (defined(CONFIG_SDK_DRIVER_NIC_R9300))
    {RTL9300_FAMILY_ID, NIC_R9300},
#endif
#if (defined(CONFIG_SDK_DRIVER_NIC_R9310))
    {RTL9310_FAMILY_ID, NIC_R9310},
#endif

};

nic_mapper_operation_t nic_ops[NIC_CTRL_END] =
{
#if defined(CONFIG_SDK_DRIVER_NIC_R8390)
    {   /* NIC_R8390 */
        .init = r8390_init,
        .pkt_tx = r8390_pkt_tx,
        .rx_start = r8390_rx_start,
        .rx_stop = r8390_rx_stop,
        .rx_register = r8390_rx_register,
        .rx_unregister = r8390_rx_unregister,
        .pkt_alloc = r8390_pkt_alloc,
        .pkt_free = r8390_pkt_free,

        .ringInfo_get = r8390_ringInfo_get,
        .cpuPortTxRxEnable_set = r8390_cpuPortTxRxEnable_set,
        .intrMask_get = r8390_intrMask_get,
        .intrMask_set = r8390_intrMask_set,
        .intrSts_get = r8390_intrSts_get,
        .intrSts_set = r8390_intrSts_set,
        .swNicRst_get = r8390_swNicRst_get,
        .swNicRst_set = r8390_swNicRst_set,
        .swQueRst_get = r8390_swQueRst_get,
        .swQueRst_set = r8390_swQueRst_set,
        .cpuL2FloodMask_add = r8390_cpuL2FloodMask_add,
        .cpuL2FloodMask_remove = r8390_cpuL2FloodMask_remove,
        .cpuForceLinkupEnable_set = r8390_cpuForceLinkupEnable_set,
        .holRingSize_set = r8390_holRingSize_set,
        .holRingCnt_get = r8390_holRingCnt_get,
        .holRingCnt_set = r8390_holRingCnt_set,
        .ntfyBaseAddr_get = r8390_ntfyBaseAddr_get,
        .ntfyBaseAddr_set = r8390_ntfyBaseAddr_set,
        .ringBaseAddr_get = r8390_ringBaseAddr_get,
        .ringBaseAddr_set = r8390_ringBaseAddr_set,
        .ringCurAddr_get = r8390_ringCurAddr_get,
        .rxTruncateLength_get = r8390_rxTruncateLength_get,
        .rxTruncateLength_set = r8390_rxTruncateLength_set,
        .nicEnable_get = r8390_nicEnable_get,
        .nicEnable_set = r8390_nicEnable_set,
        .nicTxFetch_set = r8390_nicTxFetch_set,
        .nicTxBusySts_get = r8390_nicTxBusySts_get,
        .cpuTagId_get = r8390_cpuTagId_get,
        .cpuTagId_set = r8390_cpuTagId_set,
        .supportJumboSize_get = r8390_supportJumboSize_get,
        .cpuTagFromRaw_cnvt = r8390_cpuTagFromRaw_cnvt,
        .cpuTagToRaw_cnvt = r8390_cpuTagToRaw_cnvt,
        .cpuTag_dump = r8390_cpuTag_dump,
        .rawTag_dump = r8390_rawTag_dump,

        .dbg_set = r8390_debug_set,
        .dbg_get = r8390_debug_get,
        .cntr_dump = r8390_counter_dump,
        .cntr_clear = r8390_counter_clear,
        .ringbuf_dump = r8390_bufStatus_dump,
        .pkthdr_mbuf_dump = r8390_pkthdrMbuf_dump,
        .rx_status_get = r8390_rxStatus_get,
        .nic_reset = NULL
    },
#endif
#if defined(CONFIG_SDK_DRIVER_NIC_R8380)
    {   /* NIC_R8380 */
        .init = r8380_init,
        .pkt_tx = r8380_pkt_tx,
        .rx_start = r8380_rx_start,
        .rx_stop = r8380_rx_stop,
        .rx_register = r8380_rx_register,
        .rx_unregister = r8380_rx_unregister,
        .pkt_alloc = r8380_pkt_alloc,
        .pkt_free = r8380_pkt_free,

        .ringInfo_get = r8380_ringInfo_get,
        .cpuPortTxRxEnable_set = r8380_cpuPortTxRxEnable_set,
        .intrMask_get = r8380_intrMask_get,
        .intrMask_set = r8380_intrMask_set,
        .intrSts_get = r8380_intrSts_get,
        .intrSts_set = r8380_intrSts_set,
        .swNicRst_get = r8380_swNicRst_get,
        .swNicRst_set = r8380_swNicRst_set,
        .swQueRst_get = r8380_swQueRst_get,
        .swQueRst_set = r8380_swQueRst_set,
        .cpuL2FloodMask_add = r8380_cpuL2FloodMask_add,
        .cpuL2FloodMask_remove = r8380_cpuL2FloodMask_remove,
        .cpuForceLinkupEnable_set = r8380_cpuForceLinkupEnable_set,
        .holRingSize_set = r8380_holRingSize_set,
        .holRingCnt_get = r8380_holRingCnt_get,
        .holRingCnt_set = r8380_holRingCnt_set,
        .ntfyBaseAddr_get = r8380_ntfyBaseAddr_get,
        .ntfyBaseAddr_set = r8380_ntfyBaseAddr_set,
        .ringBaseAddr_get = r8380_ringBaseAddr_get,
        .ringBaseAddr_set = r8380_ringBaseAddr_set,
        .ringCurAddr_get = r8380_ringCurAddr_get,
        .rxTruncateLength_get = r8380_rxTruncateLength_get,
        .rxTruncateLength_set = r8380_rxTruncateLength_set,
        .nicEnable_get = r8380_nicEnable_get,
        .nicEnable_set = r8380_nicEnable_set,
        .nicTxFetch_set = r8380_nicTxFetch_set,
        .nicTxBusySts_get = r8380_nicTxBusySts_get,
        .cpuTagId_get = r8380_cpuTagId_get,
        .cpuTagId_set = r8380_cpuTagId_set,
        .supportJumboSize_get = r8380_supportJumboSize_get,
        .cpuTagFromRaw_cnvt = r8380_cpuTagFromRaw_cnvt,
        .cpuTagToRaw_cnvt = r8380_cpuTagToRaw_cnvt,
        .cpuTag_dump = r8380_cpuTag_dump,
        .rawTag_dump = r8380_rawTag_dump,

        .dbg_set = r8380_debug_set,
        .dbg_get = r8380_debug_get,
        .cntr_dump = r8380_counter_dump,
        .cntr_clear = r8380_counter_clear,
        .ringbuf_dump = r8380_bufStatus_dump,
        .pkthdr_mbuf_dump = r8380_pkthdrMbuf_dump,
        .rx_status_get = r8380_rxStatus_get,
    },
#endif
#if (defined(CONFIG_SDK_DRIVER_NIC_R9300))
    {   /* NIC_R9300*/
        .init = r9300_init,
        .pkt_tx = r9300_pkt_tx,
        .rx_start = r9300_rx_start,
        .rx_stop = r9300_rx_stop,
        .rx_register = r9300_rx_register,
        .rx_unregister = r9300_rx_unregister,
        .pkt_alloc = r9300_pkt_alloc,
        .pkt_free = r9300_pkt_free,

        .ringInfo_get = r9300_ringInfo_get,
        .cpuPortTxRxEnable_set = r9300_cpuPortTxRxEnable_set,
        .intrMask_get = r9300_intrMask_get,
        .intrMask_set = r9300_intrMask_set,
        .intrSts_get = r9300_intrSts_get,
        .intrSts_set = r9300_intrSts_set,
        .swNicRst_get = r9300_swNicRst_get,
        .swNicRst_set = r9300_swNicRst_set,
        .swQueRst_get = r9300_swQueRst_get,
        .swQueRst_set = r9300_swQueRst_set,
        .cpuL2FloodMask_add = r9300_cpuL2FloodMask_add,
        .cpuL2FloodMask_remove = r9300_cpuL2FloodMask_remove,
        .cpuForceLinkupEnable_set = r9300_cpuForceLinkupEnable_set,
        .holRingSize_set = r9300_holRingSize_set,
        .holRingCnt_get = r9300_holRingCnt_get,
        .holRingCnt_set = r9300_holRingCnt_set,
        .ntfyBaseAddr_get = r9300_ntfyBaseAddr_get,
        .ntfyBaseAddr_set = r9300_ntfyBaseAddr_set,
        .ringBaseAddr_get = r9300_ringBaseAddr_get,
        .ringBaseAddr_set = r9300_ringBaseAddr_set,
        .ringCurAddr_get = r9300_ringCurAddr_get,
        .rxTruncateLength_get = r9300_rxTruncateLength_get,
        .rxTruncateLength_set = r9300_rxTruncateLength_set,
        .nicEnable_get = r9300_nicEnable_get,
        .nicEnable_set = r9300_nicEnable_set,
        .nicTxFetch_set = r9300_nicTxFetch_set,
        .nicTxBusySts_get = r9300_nicTxBusySts_get,
        .cpuTagId_get = r9300_cpuTagId_get,
        .cpuTagId_set = r9300_cpuTagId_set,
        .supportJumboSize_get = r9300_supportJumboSize_get,
        .cpuTagFromRaw_cnvt = r9300_cpuTagFromRaw_cnvt,
        .cpuTagToRaw_cnvt = r9300_cpuTagToRaw_cnvt,
        .cpuTag_dump = r9300_cpuTag_dump,
        .rawTag_dump = r9300_rawTag_dump,

        .dbg_set = r9300_debug_set,
        .dbg_get = r9300_debug_get,
        .cntr_dump = r9300_counter_dump,
        .cntr_clear = r9300_counter_clear,
        .ringbuf_dump = r9300_bufStatus_dump,
        .pkthdr_mbuf_dump = r9300_pkthdrMbuf_dump,
        .rx_status_get = r9300_rxStatus_get,
        .nic_reset = NULL
    },
#endif
#if (defined(CONFIG_SDK_DRIVER_NIC_R9310))
    {   /* NIC_R9310*/
        .init = r9310_init,
        .pkt_tx = r9310_pkt_tx,
        .rx_start = r9310_rx_start,
        .rx_stop = r9310_rx_stop,
        .rx_register = r9310_rx_register,
        .rx_unregister = r9310_rx_unregister,
        .pkt_alloc = r9310_pkt_alloc,
        .pkt_free = r9310_pkt_free,

        .ringInfo_get = r9310_ringInfo_get,
        .cpuPortTxRxEnable_set = r9310_cpuPortTxRxEnable_set,
        .intrMask_get = r9310_intrMask_get,
        .intrMask_set = r9310_intrMask_set,
        .intrSts_get = r9310_intrSts_get,
        .intrSts_set = r9310_intrSts_set,
        .swNicRst_get = r9310_swNicRst_get,
        .swNicRst_set = r9310_swNicRst_set,
        .swQueRst_get = r9310_swQueRst_get,
        .swQueRst_set = r9310_swQueRst_set,
        .cpuL2FloodMask_add = r9310_cpuL2FloodMask_add,
        .cpuL2FloodMask_remove = r9310_cpuL2FloodMask_remove,
        .cpuForceLinkupEnable_set = r9310_cpuForceLinkupEnable_set,
        .holRingSize_set = r9310_holRingSize_set,
        .holRingCnt_get = r9310_holRingCnt_get,
        .holRingCnt_set = r9310_holRingCnt_set,
        .ntfyBaseAddr_get = r9310_ntfyBaseAddr_get,
        .ntfyBaseAddr_set = r9310_ntfyBaseAddr_set,
        .ringBaseAddr_get = r9310_ringBaseAddr_get,
        .ringBaseAddr_set = r9310_ringBaseAddr_set,
        .ringCurAddr_get = r9310_ringCurAddr_get,
        .rxTruncateLength_get = r9310_rxTruncateLength_get,
        .rxTruncateLength_set = r9310_rxTruncateLength_set,
        .nicEnable_get = r9310_nicEnable_get,
        .nicEnable_set = r9310_nicEnable_set,
        .nicTxFetch_set = r9310_nicTxFetch_set,
        .nicTxBusySts_get = r9310_nicTxBusySts_get,
        .cpuTagId_get = r9310_cpuTagId_get,
        .cpuTagId_set = r9310_cpuTagId_set,
        .supportJumboSize_get = r9310_supportJumboSize_get,
        .cpuTagFromRaw_cnvt = r9310_cpuTagFromRaw_cnvt,
        .cpuTagToRaw_cnvt = r9310_cpuTagToRaw_cnvt,
        .cpuTag_dump = r9310_cpuTag_dump,
        .rawTag_dump = r9310_rawTag_dump,

        .dbg_set = r9310_debug_set,
        .dbg_get = r9310_debug_get,
        .cntr_dump = r9310_counter_dump,
        .cntr_clear = r9310_counter_clear,
        .ringbuf_dump = r9310_bufStatus_dump,
        .pkthdr_mbuf_dump = r9310_pkthdrMbuf_dump,
        .rx_status_get = r9310_rxStatus_get,
        .nic_reset = NULL
    },
#endif

};

uint32 nic_if[RTK_MAX_NUM_OF_UNIT] = {CID_GROUP_NONE};


/*
 * Function Declaration
 */

/* Function Name:
 *      nic_probe
 * Description:
 *      Probe nic module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
nic_probe(uint32 unit)
{
    uint32      i;
    uint32      cid;

    RT_INIT_MSG("  NIC probe (unit %u)\n", unit);

    cid = HWP_CHIP_ID(unit);

    nic_if[unit] = CID_GROUP_NONE;
    for (i=0; i<NIC_DB_SIZE; i++)
    {
        if (CID_PREFIX_MATCH(nic_db[i], cid))
        {
            nic_if[unit] = nic_db[i].gid;
            return RT_ERR_OK;
        }
    }

    return RT_ERR_FAILED;

} /* end of nic_probe */

