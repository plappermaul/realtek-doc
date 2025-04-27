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
 * Purpose : Define diag shell commands for counter
 *
 * Feature : The file includes the following module and sub-modules
 *           1) counter commands.
 *
 */

#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/stat.h>
#include <rtk/stack.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_counter.h>
  #include <rtrpc/rtrpc_stack.h>
#endif

/*
NOTE: check the mib counter name is "TX_egressQueueXDropPktsRT",
          and return the counter index
*/
uint32 _is_egrQueueDropName(char *pBuf, uint32 *pCntr_idx)
{
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    char *pStr;
    uint32 size, idx;

    if (NULL == pBuf || NULL == pCntr_idx)
        return FALSE;

    pStr = pBuf;
    size = sizeof("TX_egressQueue") -1;
    if (0 == osal_strcmp(pStr, "TX_egressQueue0DropPktsRT"))
    {
        *pCntr_idx = 0;
        return TRUE;
    }
    else
    {
        if (0 == osal_memcmp(pStr, "TX_egressQueue", size))
        {
            idx = atoi(pStr+ size);
            if ((idx >= 32) || (idx == 0))
                return FALSE;

            size +=1;
            if (idx/10 > 0)
                size +=1;

            if (0 == osal_strncmp(pStr+size, "DropPktsRT", sizeof("DropPktsRT")))
            {
                *pCntr_idx = idx;
                return TRUE;
            }
        }
    }
#endif

    return FALSE;
}


#ifdef CMD_MIB_SET_STATE_DISABLE_ENABLE
/*
 * mib set state ( disable | enable )
 */
cparser_result_t
cparser_cmd_mib_set_state_disable_enable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_PARSE_STATE(3, state);

    DIAG_UTIL_MPRINTF("mib state: %s\n", text_state[state]);
    DIAG_UTIL_ERR_CHK(rtk_stat_enable_set(unit, state), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mib_set_state_disable_enable */
#endif

#ifdef CMD_MIB_GET_STATE
/*
 * mib get state
 */
cparser_result_t
cparser_cmd_mib_get_state(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t state;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_stat_enable_get(unit, &state), ret);
    DIAG_UTIL_MPRINTF("mib state: %s\n", text_state[state]);

    return CPARSER_OK;
}   /* end of cparser_cmd_mib_get_state */
#endif

#ifdef CMD_MIB_DUMP_COUNTER_PORT_PORTS_ALL
/*
 * mib dump counter port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_mib_dump_counter_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_stat_port_cntr_t port_cntrs;
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    rtk_stat_flexCntSet_t range0, range1;
#endif

#if defined(CONFIG_SDK_RTL9300)
    uint32  qid, qNum = 8;
    rtk_switch_devInfo_t devInfo;
#endif

    osal_memset(&port_cntrs, 0, sizeof(rtk_stat_port_cntr_t));

    DIAG_UTIL_FUNC_INIT(unit);

#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_stat_flexibleCntRange_get(unit, 0, &range0), ret);
        DIAG_UTIL_ERR_CHK(rtk_stat_flexibleCntRange_get(unit, 1, &range1), ret);
    }
#endif

#if defined(CONFIG_SDK_RTL9300)
    osal_memset(&devInfo,0,sizeof(rtk_switch_devInfo_t));
    DIAG_UTIL_ERR_CHK(diag_om_get_deviceInfo(unit, &devInfo), ret);
#endif

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_stat_port_getAll(unit, port, &port_cntrs), ret);

#if defined(CONFIG_SDK_RTL8390)
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            diag_util_printf("Port %2u Counter\n", port);
            diag_util_printf("\t------------- Bridge/Interface MIB -----------\n");
            diag_util_printf("\tdot1dTpPortInDiscards : %u\n", port_cntrs.dot1dTpPortInDiscards);
            diag_util_printf("\tifInOctets : %llu\n", port_cntrs.ifInOctets);
            diag_util_printf("\tifHCInOctets : %llu\n", port_cntrs.ifHCInOctets);
            diag_util_printf("\tifInUcastPkts : %u\n", port_cntrs.ifInUcastPkts);
            diag_util_printf("\tifInMulticastPkts : %u\n", port_cntrs.ifInMulticastPkts);
            diag_util_printf("\tifInBroadcastPkts : %u\n", port_cntrs.ifInBroadcastPkts);
            diag_util_printf("\tifOutOctets : %llu\n", port_cntrs.ifOutOctets);
            diag_util_printf("\tifHCOutOctets : %llu\n", port_cntrs.ifHCOutOctets);
            diag_util_printf("\tifOutUcastPkts : %u\n", port_cntrs.ifOutUcastPkts);
            diag_util_printf("\tifOutMulticastPkts : %u\n", port_cntrs.ifOutMulticastPkts);
            diag_util_printf("\tifOutBroadcastPkts : %u\n", port_cntrs.ifOutBrocastPkts);
            diag_util_printf("\tifOutDiscards : %u\n", port_cntrs.ifOutDiscards);
            diag_util_printf("\t------------- Etherlike MIB ------------------\n");
            diag_util_printf("\tdot3StatsSingleCollisionFrames : %u\n", port_cntrs.dot3StatsSingleCollisionFrames);
            diag_util_printf("\tdot3StatsMultipleCollisionFrames : %u\n", port_cntrs.dot3StatsMultipleCollisionFrames);
            diag_util_printf("\tdot3StatsDeferredTransmissions : %u\n", port_cntrs.dot3StatsDeferredTransmissions);
            diag_util_printf("\tdot3StatsLateCollisions : %u\n", port_cntrs.dot3StatsLateCollisions);
            diag_util_printf("\tdot3StatsExcessiveCollisions : %u\n", port_cntrs.dot3StatsExcessiveCollisions);
            diag_util_printf("\tdot3StatsSymbolErrors : %u\n", port_cntrs.dot3StatsSymbolErrors);
            diag_util_printf("\tdot3ControlInUnknownOpcodes : %u\n", port_cntrs.dot3ControlInUnknownOpcodes);
            diag_util_printf("\tdot3InPauseFrames : %u\n", port_cntrs.dot3InPauseFrames);
            diag_util_printf("\tdot3OutPauseFrames : %u\n", port_cntrs.dot3OutPauseFrames);
            diag_util_printf("\t------------- RMON MIB -----------------------\n");
            diag_util_printf("\tetherStatsDropEvents : %u\n", port_cntrs.etherStatsDropEvents);
            diag_util_printf("\tetherStatsBroadcastPkts : %u\n", port_cntrs.etherStatsBroadcastPkts);
            diag_util_printf("\tetherStatsMulticastPkts : %u\n", port_cntrs.etherStatsMulticastPkts);
            diag_util_printf("\tetherStatsCRCAlignErrors : %u\n", port_cntrs.etherStatsCRCAlignErrors);
            diag_util_printf("\tetherStatsUndersizePkts : %u\n", port_cntrs.etherStatsUndersizePkts);
            diag_util_printf("\tetherStatsOversizePkts : %u\n", port_cntrs.etherStatsOversizePkts);
            diag_util_printf("\tetherStatsFragments : %u\n", port_cntrs.etherStatsFragments);
            diag_util_printf("\tetherStatsJabbers : %u\n", port_cntrs.etherStatsJabbers);
            diag_util_printf("\tetherStatsCollisions : %u\n", port_cntrs.etherStatsCollisions);
            diag_util_printf("\tetherStatsPkts64Octets : %u\n", port_cntrs.etherStatsPkts64Octets);
            diag_util_printf("\tetherStatsPkts65to127Octets : %u\n", port_cntrs.etherStatsPkts65to127Octets);
            diag_util_printf("\tetherStatsPkts128to255Octets : %u\n", port_cntrs.etherStatsPkts128to255Octets);
            diag_util_printf("\tetherStatsPkts256to511Octets : %u\n", port_cntrs.etherStatsPkts256to511Octets);
            diag_util_printf("\tetherStatsPkts512to1023Octets : %u\n", port_cntrs.etherStatsPkts512to1023Octets);
            diag_util_printf("\tetherStatsPkts1024to1518Octets : %u\n", port_cntrs.etherStatsPkts1024to1518Octets);
            diag_util_printf("\t------------- RMON(RX)/Private MIB -----------\n");
            diag_util_printf("\tRX_etherStatsUndersizePkts : %u\n", port_cntrs.etherStatsRxUndersizePkts);
            diag_util_printf("\tRX_etherStatsUndersizeDropPkts : %u\n", port_cntrs.etherStatsRxUndersizeDropPkts);
            diag_util_printf("\tRX_etherStatsOversizePkts : %u\n", port_cntrs.etherStatsRxOversizePkts);
            diag_util_printf("\tRX_etherStatsPkts64Octets : %u\n", port_cntrs.etherStatsRxPkts64Octets);
            diag_util_printf("\tRX_etherStatsPkts65to127Octets : %u\n", port_cntrs.etherStatsRxPkts65to127Octets);
            diag_util_printf("\tRX_etherStatsPkts128to255Octets : %u\n", port_cntrs.etherStatsRxPkts128to255Octets);
            diag_util_printf("\tRX_etherStatsPkts256to511Octets : %u\n", port_cntrs.etherStatsRxPkts256to511Octets);
            diag_util_printf("\tRX_etherStatsPkts512to1023Octets : %u\n", port_cntrs.etherStatsRxPkts512to1023Octets);
            diag_util_printf("\tRX_etherStatsPkts1024to1518Octets : %u\n", port_cntrs.etherStatsRxPkts1024to1518Octets);
            diag_util_printf("\tRX_etherStatsPkts1519toMaxOctets : %u\n", port_cntrs.etherStatsRxPkts1519toMaxOctets);
            diag_util_printf("\tRX_lengthFieldError : %u\n", port_cntrs.rxLengthFieldError);
            diag_util_printf("\tRX_falseCarrierTimes : %u\n", port_cntrs.rxFalseCarrierTimes);
            diag_util_printf("\tRX_underSizeOctets : %u\n", port_cntrs.rxUnderSizeOctets);
            diag_util_printf("\tRX_FramingErrors : %u\n", port_cntrs.rxFramingErrors);
            diag_util_printf("\tRX_MacDiscards : %u\n", port_cntrs.rxMacDiscards);
            diag_util_printf("\t------------- RMON(TX)/Private MIB -----------\n");
            diag_util_printf("\tTX_etherStatsBroadcastPkts : %u\n", port_cntrs.etherStatsTxBroadcastPkts);
            diag_util_printf("\tTX_etherStatsMulticastPkts : %u\n", port_cntrs.etherStatsTxMulticastPkts);
            diag_util_printf("\tTX_etherStatsUndersizePkts : %u\n", port_cntrs.etherStatsTxUndersizePkts);
            diag_util_printf("\tTX_etherStatsOversizePkts : %u\n", port_cntrs.etherStatsTxOversizePkts);
            diag_util_printf("\tTX_etherStatsPkts64Octets : %u\n", port_cntrs.etherStatsTxPkts64Octets);
            diag_util_printf("\tTX_etherStatsPkts65to127Octets : %u\n", port_cntrs.etherStatsTxPkts65to127Octets);
            diag_util_printf("\tTX_etherStatsPkts128to255Octets : %u\n", port_cntrs.etherStatsTxPkts128to255Octets);
            diag_util_printf("\tTX_etherStatsPkts256to511Octets : %u\n", port_cntrs.etherStatsTxPkts256to511Octets);
            diag_util_printf("\tTX_etherStatsPkts512to1023Octets : %u\n", port_cntrs.etherStatsTxPkts512to1023Octets);
            diag_util_printf("\tTX_etherStatsPkts1024to1518Octets : %u\n", port_cntrs.etherStatsTxPkts1024to1518Octets);
            diag_util_printf("\tTX_etherStatsPkts1519toMaxOctets : %u\n", port_cntrs.etherStatsTxPkts1519toMaxOctets);
            diag_util_printf("\tTX_etherStatsFragments : %u\n", port_cntrs.txEtherStatsFragments);
            diag_util_printf("\tTX_etherStatsJabbers : %u\n", port_cntrs.txEtherStatsJabbers);
            diag_util_printf("\tTX_etherStatsCRCAlignError : %u\n", port_cntrs.txEtherStatsCRCAlignErrors);
        }
#endif

#if defined(CONFIG_SDK_RTL8380)
         if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_printf("Port %2u Counter\n", port);
            diag_util_printf("\tdot1dTpPortInDiscards : %u\n", port_cntrs.dot1dTpPortInDiscards);
            diag_util_printf("\tifInOctets : %llu\n", port_cntrs.ifInOctets);
            diag_util_printf("\tifHCInOctets : %llu\n", port_cntrs.ifHCInOctets);
            diag_util_printf("\tifInUcastPkts : %u\n", port_cntrs.ifInUcastPkts);
            diag_util_printf("\tifInMulticastPkts : %u\n", port_cntrs.ifInMulticastPkts);
            diag_util_printf("\tifInBroadcastPkts : %u\n", port_cntrs.ifInBroadcastPkts);
            diag_util_printf("\tifOutOctets : %llu\n", port_cntrs.ifOutOctets);
            diag_util_printf("\tifHCOutOctets : %llu\n", port_cntrs.ifHCOutOctets);
            diag_util_printf("\tifOutUcastPkts : %u\n", port_cntrs.ifOutUcastPkts);
            diag_util_printf("\tifOutMulticastPkts : %u\n", port_cntrs.ifOutMulticastPkts);
            diag_util_printf("\tifOutBrocastPkts : %u\n", port_cntrs.ifOutBrocastPkts);
            diag_util_printf("\tifOutDiscards : %u\n", port_cntrs.ifOutDiscards);
            diag_util_printf("\tdot3StatsSingleCollisionFrames : %u\n", port_cntrs.dot3StatsSingleCollisionFrames);
            diag_util_printf("\tdot3StatsMultipleCollisionFrames : %u\n", port_cntrs.dot3StatsMultipleCollisionFrames);
            diag_util_printf("\tdot3StatsDeferredTransmissions : %u\n", port_cntrs.dot3StatsDeferredTransmissions);
            diag_util_printf("\tdot3StatsLateCollisions : %u\n", port_cntrs.dot3StatsLateCollisions);
            diag_util_printf("\tdot3StatsExcessiveCollisions : %u\n", port_cntrs.dot3StatsExcessiveCollisions);
            diag_util_printf("\tdot3StatsSymbolErrors : %u\n", port_cntrs.dot3StatsSymbolErrors);
            diag_util_printf("\tdot3ControlInUnknownOpcodes : %u\n", port_cntrs.dot3ControlInUnknownOpcodes);
            diag_util_printf("\tdot3InPauseFrames : %u\n", port_cntrs.dot3InPauseFrames);
            diag_util_printf("\tdot3OutPauseFrames : %u\n", port_cntrs.dot3OutPauseFrames);
            diag_util_printf("\tetherStatsDropEvents : %u\n", port_cntrs.etherStatsDropEvents);
            diag_util_printf("\tetherStatsBroadcastPkts : %u\n", port_cntrs.etherStatsBroadcastPkts);
            diag_util_printf("\tTX_etherStatsBroadcastPkts : %u\n", port_cntrs.etherStatsTxBroadcastPkts);
            diag_util_printf("\tetherStatsMulticastPkts : %u\n", port_cntrs.etherStatsMulticastPkts);
            diag_util_printf("\tTX_etherStatsMulticastPkts : %u\n", port_cntrs.etherStatsTxMulticastPkts);
            diag_util_printf("\tetherStatsCRCAlignErrors : %u\n", port_cntrs.etherStatsCRCAlignErrors);
            diag_util_printf("\tetherStatsUndersizePkts : %u\n", port_cntrs.etherStatsUndersizePkts);
            diag_util_printf("\tRX_etherStatsUndersizePkts : %u\n", port_cntrs.etherStatsRxUndersizePkts);
            diag_util_printf("\tRX_etherStatsUndersizeDropPkts : %u\n", port_cntrs.etherStatsRxUndersizeDropPkts);
            diag_util_printf("\tTX_etherStatsUndersizePkts : %u\n", port_cntrs.etherStatsTxUndersizePkts);
            diag_util_printf("\tetherStatsOversizePkts : %u\n", port_cntrs.etherStatsOversizePkts);
            diag_util_printf("\tRX_etherStatsOversizePkts : %u\n", port_cntrs.etherStatsRxOversizePkts);
            diag_util_printf("\tTX_etherStatsOversizePkts : %u\n", port_cntrs.etherStatsTxOversizePkts);
            diag_util_printf("\tetherStatsFragments : %u\n", port_cntrs.etherStatsFragments);
            diag_util_printf("\tetherStatsJabbers : %u\n", port_cntrs.etherStatsJabbers);
            diag_util_printf("\tetherStatsCollisions : %u\n", port_cntrs.etherStatsCollisions);
            diag_util_printf("\tetherStatsPkts64Octets : %u\n", port_cntrs.etherStatsPkts64Octets);
            diag_util_printf("\tRX_etherStatsPkts64Octets : %u\n", port_cntrs.etherStatsRxPkts64Octets);
            diag_util_printf("\tTX_etherStatsPkts64Octets : %u\n", port_cntrs.etherStatsTxPkts64Octets);
            diag_util_printf("\tetherStatsPkts65to127Octets : %u\n", port_cntrs.etherStatsPkts65to127Octets);
            diag_util_printf("\tRX_etherStatsPkts65to127Octets : %u\n", port_cntrs.etherStatsRxPkts65to127Octets);
            diag_util_printf("\tTX_etherStatsPkts65to127Octets : %u\n", port_cntrs.etherStatsTxPkts65to127Octets);
            diag_util_printf("\tetherStatsPkts128to255Octets : %u\n", port_cntrs.etherStatsPkts128to255Octets);
            diag_util_printf("\tRX_etherStatsPkts128to255Octets : %u\n", port_cntrs.etherStatsRxPkts128to255Octets);
            diag_util_printf("\tTX_etherStatsPkts128to255Octets : %u\n", port_cntrs.etherStatsTxPkts128to255Octets);
            diag_util_printf("\tetherStatsPkts256to511Octets : %u\n", port_cntrs.etherStatsPkts256to511Octets);
            diag_util_printf("\tRX_etherStatsPkts256to511Octets : %u\n", port_cntrs.etherStatsRxPkts256to511Octets);
            diag_util_printf("\tTX_etherStatsPkts256to511Octets : %u\n", port_cntrs.etherStatsTxPkts256to511Octets);
            diag_util_printf("\tetherStatsPkts512to1023Octets : %u\n", port_cntrs.etherStatsPkts512to1023Octets);
            diag_util_printf("\tRX_etherStatsPkts512to1023Octets : %u\n", port_cntrs.etherStatsRxPkts512to1023Octets);
            diag_util_printf("\tTX_etherStatsPkts512to1023Octets : %u\n", port_cntrs.etherStatsTxPkts512to1023Octets);
            diag_util_printf("\tetherStatsPkts1024to1518Octets : %u\n", port_cntrs.etherStatsPkts1024to1518Octets);
            diag_util_printf("\tRX_etherStatsPkts1024to1518Octets : %u\n", port_cntrs.etherStatsRxPkts1024to1518Octets);
            diag_util_printf("\tTX_etherStatsPkts1024to1518Octets : %u\n", port_cntrs.etherStatsTxPkts1024to1518Octets);
            diag_util_printf("\tRX_etherStatsPkts1519toMaxOctets : %u\n", port_cntrs.etherStatsRxPkts1519toMaxOctets);
            diag_util_printf("\tTX_etherStatsPkts1519toMaxOctets : %u\n", port_cntrs.etherStatsTxPkts1519toMaxOctets);
            diag_util_printf("\tRX_MacDiscards : %u\n", port_cntrs.rxMacDiscards);
        }
#endif

#if defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            diag_util_printf("Port %2u Counter\n", port);
            diag_util_printf("\t------------- Bridge/Interface MIB -----------\n");
            diag_util_printf("\tdot1dTpPortInDiscards : %u\n", port_cntrs.dot1dTpPortInDiscards);
            diag_util_printf("\tifInOctets : %llu\n", port_cntrs.ifInOctets);
            diag_util_printf("\tifHCInOctets : %llu\n", port_cntrs.ifHCInOctets);
            diag_util_printf("\tifInUcastPkts : %u\n", port_cntrs.ifInUcastPkts);
            diag_util_printf("\tifHCInUcastPkts : %llu\n", port_cntrs.ifHCInUcastPkts);
            diag_util_printf("\tifInMulticastPkts : %u\n", port_cntrs.ifInMulticastPkts);
            diag_util_printf("\tifHCInMulticastPkts : %llu\n", port_cntrs.ifHCInMulticastPkts);
            diag_util_printf("\tifInBroadcastPkts : %u\n", port_cntrs.ifInBroadcastPkts);
            diag_util_printf("\tifHCInBroadcastPkts : %llu\n", port_cntrs.ifHCInBroadcastPkts);
            diag_util_printf("\tifOutOctets : %llu\n", port_cntrs.ifOutOctets);
            diag_util_printf("\tifHCOutOctets : %llu\n", port_cntrs.ifHCOutOctets);
            diag_util_printf("\tifOutUcastPkts : %u\n", port_cntrs.ifOutUcastPkts);
            diag_util_printf("\tifHCOutUcastPkts : %llu\n", port_cntrs.ifHCOutUcastPkts);
            diag_util_printf("\tifOutMulticastPkts : %u\n", port_cntrs.ifOutMulticastPkts);
            diag_util_printf("\tifHCOutMulticastPkts : %llu\n", port_cntrs.ifHCOutMulticastPkts);
            diag_util_printf("\tifOutBroadcastPkts : %u\n", port_cntrs.ifOutBrocastPkts);
            diag_util_printf("\tifHCOutBroadcastPkts : %llu\n", port_cntrs.ifHCOutBrocastPkts);
            diag_util_printf("\tifOutDiscards : %u\n", port_cntrs.ifOutDiscards);
            diag_util_printf("\t------------- Etherlike MIB ------------------\n");
            diag_util_printf("\tdot3StatsSingleCollisionFrames : %u\n", port_cntrs.dot3StatsSingleCollisionFrames);
            diag_util_printf("\tdot3StatsMultipleCollisionFrames : %u\n", port_cntrs.dot3StatsMultipleCollisionFrames);
            diag_util_printf("\tdot3StatsDeferredTransmissions : %u\n", port_cntrs.dot3StatsDeferredTransmissions);
            diag_util_printf("\tdot3StatsLateCollisions : %u\n", port_cntrs.dot3StatsLateCollisions);
            diag_util_printf("\tdot3StatsExcessiveCollisions : %u\n", port_cntrs.dot3StatsExcessiveCollisions);
            diag_util_printf("\tdot3StatsSymbolErrors : %u\n", port_cntrs.dot3StatsSymbolErrors);
            diag_util_printf("\tdot3ControlInUnknownOpcodes : %u\n", port_cntrs.dot3ControlInUnknownOpcodes);
            diag_util_printf("\tdot3InPauseFrames : %u\n", port_cntrs.dot3InPauseFrames);
            diag_util_printf("\tdot3OutPauseFrames : %u\n", port_cntrs.dot3OutPauseFrames);
            diag_util_printf("\t------------- RMON (RX+TX) MIB -----------------------\n");
            diag_util_printf("\tetherStatsDropEvents : %u\n", port_cntrs.etherStatsDropEvents);
            diag_util_printf("\tetherStatsBroadcastPkts : %u\n", port_cntrs.etherStatsBroadcastPkts);
            diag_util_printf("\tetherStatsMulticastPkts : %u\n", port_cntrs.etherStatsMulticastPkts);
            diag_util_printf("\tetherStatsCRCAlignErrors : %u\n", port_cntrs.etherStatsCRCAlignErrors);
            diag_util_printf("\tetherStatsUndersizePkts : %u\n", port_cntrs.etherStatsUndersizePkts);
            diag_util_printf("\tetherStatsOversizePkts : %u\n", port_cntrs.etherStatsOversizePkts);
            diag_util_printf("\tetherStatsFragments : %u\n", port_cntrs.etherStatsFragments);
            diag_util_printf("\tetherStatsJabbers : %u\n", port_cntrs.etherStatsJabbers);
            diag_util_printf("\tetherStatsCollisions : %u\n", port_cntrs.etherStatsCollisions);
            diag_util_printf("\tetherStatsPkts64Octets : %u\n", port_cntrs.etherStatsPkts64Octets);
            diag_util_printf("\tetherStatsPkts65to127Octets : %u\n", port_cntrs.etherStatsPkts65to127Octets);
            diag_util_printf("\tetherStatsPkts128to255Octets : %u\n", port_cntrs.etherStatsPkts128to255Octets);
            diag_util_printf("\tetherStatsPkts256to511Octets : %u\n", port_cntrs.etherStatsPkts256to511Octets);
            diag_util_printf("\tetherStatsPkts512to1023Octets : %u\n", port_cntrs.etherStatsPkts512to1023Octets);
            diag_util_printf("\tetherStatsPkts1024to1518Octets : %u\n", port_cntrs.etherStatsPkts1024to1518Octets);
            diag_util_printf("\t------------- RMON(RX) MIB -----------\n");
            diag_util_printf("\tRX_etherStatsDropEvents : %u\n", port_cntrs.etherStatsDropEvents);
            diag_util_printf("\tRX_etherStatsBroadcastPkts : %u\n", port_cntrs.ifInBroadcastPkts);
            diag_util_printf("\tRX_etherStatsMulticastPkts : %u\n", port_cntrs.ifInMulticastPkts);
            diag_util_printf("\tRX_etherStatsCRCAlignErrors : %u\n", port_cntrs.rxEtherStatsCRCAlignErrors);
            diag_util_printf("\tRX_etherStatsUndersizePkts : %u\n", port_cntrs.etherStatsRxUndersizePkts);
            diag_util_printf("\tRX_etherStatsOversizePkts : %u\n", port_cntrs.etherStatsRxOversizePkts);
            diag_util_printf("\tRX_etherStatsFragments : %u\n", port_cntrs.rxEtherStatsFragments);
            diag_util_printf("\tRX_etherStatsJabbers : %u\n", port_cntrs.rxEtherStatsJabbers);
            diag_util_printf("\tRX_etherStatsPkts64Octets : %u\n", port_cntrs.etherStatsRxPkts64Octets);
            diag_util_printf("\tRX_etherStatsPkts65to127Octets : %u\n", port_cntrs.etherStatsRxPkts65to127Octets);
            diag_util_printf("\tRX_etherStatsPkts128to255Octets : %u\n", port_cntrs.etherStatsRxPkts128to255Octets);
            diag_util_printf("\tRX_etherStatsPkts256to511Octets : %u\n", port_cntrs.etherStatsRxPkts256to511Octets);
            diag_util_printf("\tRX_etherStatsPkts512to1023Octets : %u\n", port_cntrs.etherStatsRxPkts512to1023Octets);
            diag_util_printf("\tRX_etherStatsPkts1024to1518Octets : %u\n", port_cntrs.etherStatsRxPkts1024to1518Octets);
            diag_util_printf("\t------------- RMON(TX) MIB -----------\n");
            diag_util_printf("\tTX_etherStatsBroadcastPkts : %u\n", port_cntrs.etherStatsTxBroadcastPkts);
            diag_util_printf("\tTX_etherStatsMulticastPkts : %u\n", port_cntrs.etherStatsTxMulticastPkts);
            diag_util_printf("\tTX_etherStatsUndersizePkts : %u\n", port_cntrs.etherStatsTxUndersizePkts);
            diag_util_printf("\tTX_etherStatsCRCAlignErrors : %u\n", port_cntrs.txEtherStatsCRCAlignErrors);
            diag_util_printf("\tTX_etherStatsOversizePkts : %u\n", port_cntrs.etherStatsTxOversizePkts);
            diag_util_printf("\tTX_etherStatsFragments : %u\n", port_cntrs.txEtherStatsFragments);
            diag_util_printf("\tTX_etherStatsJabbers : %u\n", port_cntrs.txEtherStatsJabbers);
            diag_util_printf("\tTX_etherStatsCollisions : %u\n", port_cntrs.etherStatsCollisions);
            diag_util_printf("\tTX_etherStatsPkts64Octets : %u\n", port_cntrs.etherStatsTxPkts64Octets);
            diag_util_printf("\tTX_etherStatsPkts65to127Octets : %u\n", port_cntrs.etherStatsTxPkts65to127Octets);
            diag_util_printf("\tTX_etherStatsPkts128to255Octets : %u\n", port_cntrs.etherStatsTxPkts128to255Octets);
            diag_util_printf("\tTX_etherStatsPkts256to511Octets : %u\n", port_cntrs.etherStatsTxPkts256to511Octets);
            diag_util_printf("\tTX_etherStatsPkts512to1023Octets : %u\n", port_cntrs.etherStatsTxPkts512to1023Octets);
            diag_util_printf("\tTX_etherStatsPkts1024to1518Octets : %u\n", port_cntrs.etherStatsTxPkts1024to1518Octets);
            diag_util_printf("\t------------- Private MIB -----------\n");
            diag_util_printf("\tRX_etherStatsUndersizeDropPktsRT : %u\n", port_cntrs.etherStatsRxUndersizeDropPkts);
            diag_util_printf("\tRX_etherStatsPkts1519toMaxOctetsRT : %u\n", port_cntrs.etherStatsRxPkts1519toMaxOctets);
            diag_util_printf("\tTX_etherStatsPkts1519toMaxOctetsRT : %u\n", port_cntrs.etherStatsTxPkts1519toMaxOctets);
            diag_util_printf("\tRX_etherStatsPktsOverMaxOctetsRT : %u\n", port_cntrs.etherStatsRxPktsOverMaxOctets);
            diag_util_printf("\tTX_etherStatsPktsOverMaxOctetsRT : %u\n", port_cntrs.etherStatsTxPktsOverMaxOctets);
            diag_util_printf("\tRX_etherStatsPktsFlexibleOctetsSet0RT (%u~%u) : %u\n", range0.len_min, range0.len_max, port_cntrs.rxEtherStatsPktsFlexibleOctetsSet0RT);
            diag_util_printf("\tTX_etherStatsPktsFlexibleOctetsSet0RT (%u~%u) : %u\n", range0.len_min, range0.len_max, port_cntrs.txEtherStatsPktsFlexibleOctetsSet0RT);
            diag_util_printf("\tRX_etherStatsPktsFlexibleOctetsSet1RT (%u~%u) : %u\n", range1.len_min, range1.len_max, port_cntrs.rxEtherStatsPktsFlexibleOctetsSet1RT);
            diag_util_printf("\tTX_etherStatsPktsFlexibleOctetsSet1RT (%u~%u) : %u\n", range1.len_min, range1.len_max, port_cntrs.txEtherStatsPktsFlexibleOctetsSet1RT);
            diag_util_printf("\tRX_etherStatsPktsFlexibleOctetsCRCSet0RT (%u~%u) : %u\n", range0.len_min, range0.len_max, port_cntrs.rxEtherStatsPktsFlexibleOctetsCRCSet0RT);
            diag_util_printf("\tTX_etherStatsPktsFlexibleOctetsCRCSet0RT (%u~%u) : %u\n", range0.len_min, range0.len_max, port_cntrs.txetherStatsPktsFlexibleOctetsCRCSet0RT);
            diag_util_printf("\tRX_etherStatsPktsFlexibleOctetsCRCSet1RT (%u~%u) : %u\n", range1.len_min, range1.len_max, port_cntrs.rxEtherStatsPktsFlexibleOctetsCRCSet1RT);
            diag_util_printf("\tTX_etherStatsPktsFlexibleOctetsCRCSet1RT (%u~%u) : %u\n", range1.len_min, range1.len_max, port_cntrs.txetherStatsPktsFlexibleOctetsCRCSet1RT);
            diag_util_printf("\tRX_lengthFieldErrorRT : %u\n", port_cntrs.rxLengthFieldError);
            diag_util_printf("\tRX_falseCarrierTimesRT : %u\n", port_cntrs.rxFalseCarrierTimes);
            diag_util_printf("\tRX_underSizeOctetsRT : %u\n", port_cntrs.rxUnderSizeOctets);
            diag_util_printf("\tRX_FramingErrorsRT : %u\n", port_cntrs.rxFramingErrors);
            diag_util_printf("\tRX_MacIPGShortDropRT : %u\n", port_cntrs.rxMacIPGShortDrop);
            diag_util_printf("\tRX_MacDiscardsRT : %u\n", port_cntrs.rxMacDiscards);
            diag_util_printf("\tTX_egressQueue0SentPktsRT : %u\n", port_cntrs.egrQueue0OutPkts);
            diag_util_printf("\tTX_egressQueue1SentPktsRT : %u\n", port_cntrs.egrQueue1OutPkts);
            diag_util_printf("\tTX_egressQueue2SentPktsRT : %u\n", port_cntrs.egrQueue2OutPkts);
            diag_util_printf("\tTX_egressQueue3SentPktsRT : %u\n", port_cntrs.egrQueue3OutPkts);
            diag_util_printf("\tTX_egressQueue4SentPktsRT : %u\n", port_cntrs.egrQueue4OutPkts);
            diag_util_printf("\tTX_egressQueue5SentPktsRT : %u\n", port_cntrs.egrQueue5OutPkts);
            diag_util_printf("\tTX_egressQueue6SentPktsRT : %u\n", port_cntrs.egrQueue6OutPkts);
            diag_util_printf("\tTX_egressQueue7SentPktsRT : %u\n", port_cntrs.egrQueue7OutPkts);
            diag_util_printf("\tTX_egressQueue0DropSentPktsRT : %u\n", port_cntrs.egrQueue0DropPkts);
            diag_util_printf("\tTX_egressQueue1DropSentPktsRT : %u\n", port_cntrs.egrQueue1DropPkts);
            diag_util_printf("\tTX_egressQueue2DropSentPktsRT : %u\n", port_cntrs.egrQueue2DropPkts);
            diag_util_printf("\tTX_egressQueue3DropSentPktsRT : %u\n", port_cntrs.egrQueue3DropPkts);
            diag_util_printf("\tTX_egressQueue4DropSentPktsRT : %u\n", port_cntrs.egrQueue4DropPkts);
            diag_util_printf("\tTX_egressQueue5DropSentPktsRT : %u\n", port_cntrs.egrQueue5DropPkts);
            diag_util_printf("\tTX_egressQueue6DropSentPktsRT : %u\n", port_cntrs.egrQueue6DropPkts);
            diag_util_printf("\tTX_egressQueue7DropSentPktsRT : %u\n", port_cntrs.egrQueue7DropPkts);
        }
#endif

#if defined(CONFIG_SDK_RTL9300)
            if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            {
                diag_util_printf("Port %2u Counter\n", port);
                diag_util_printf("\t------------- Bridge/Interface MIB -----------\n");
                diag_util_printf("\tdot1dTpPortInDiscards : %u\n", port_cntrs.dot1dTpPortInDiscards);
                diag_util_printf("\tifInOctets : %llu\n", port_cntrs.ifInOctets);
                diag_util_printf("\tifHCInOctets : %llu\n", port_cntrs.ifHCInOctets);
                diag_util_printf("\tifInUcastPkts : %u\n", port_cntrs.ifInUcastPkts);
                diag_util_printf("\tifHCInUcastPkts : %llu\n", port_cntrs.ifHCInUcastPkts);
                diag_util_printf("\tifInMulticastPkts : %u\n", port_cntrs.ifInMulticastPkts);
                diag_util_printf("\tifHCInMulticastPkts : %llu\n", port_cntrs.ifHCInMulticastPkts);
                diag_util_printf("\tifInBroadcastPkts : %u\n", port_cntrs.ifInBroadcastPkts);
                diag_util_printf("\tifHCInBroadcastPkts : %llu\n", port_cntrs.ifHCInBroadcastPkts);
                diag_util_printf("\tifOutOctets : %llu\n", port_cntrs.ifOutOctets);
                diag_util_printf("\tifHCOutOctets : %llu\n", port_cntrs.ifHCOutOctets);
                diag_util_printf("\tifOutUcastPkts : %u\n", port_cntrs.ifOutUcastPkts);
                diag_util_printf("\tifHCOutUcastPkts : %llu\n", port_cntrs.ifHCOutUcastPkts);
                diag_util_printf("\tifOutMulticastPkts : %u\n", port_cntrs.ifOutMulticastPkts);
                diag_util_printf("\tifHCOutMulticastPkts : %llu\n", port_cntrs.ifHCOutMulticastPkts);
                diag_util_printf("\tifOutBroadcastPkts : %u\n", port_cntrs.ifOutBrocastPkts);
                diag_util_printf("\tifHCOutBroadcastPkts : %llu\n", port_cntrs.ifHCOutBrocastPkts);
                diag_util_printf("\tifOutDiscards : %u\n", port_cntrs.ifOutDiscards);
                diag_util_printf("\t------------- Etherlike MIB ------------------\n");
                diag_util_printf("\tdot3StatsSingleCollisionFrames : %u\n", port_cntrs.dot3StatsSingleCollisionFrames);
                diag_util_printf("\tdot3StatsMultipleCollisionFrames : %u\n", port_cntrs.dot3StatsMultipleCollisionFrames);
                diag_util_printf("\tdot3StatsDeferredTransmissions : %u\n", port_cntrs.dot3StatsDeferredTransmissions);
                diag_util_printf("\tdot3StatsLateCollisions : %u\n", port_cntrs.dot3StatsLateCollisions);
                diag_util_printf("\tdot3StatsExcessiveCollisions : %u\n", port_cntrs.dot3StatsExcessiveCollisions);
                diag_util_printf("\tdot3StatsSymbolErrors : %u\n", port_cntrs.dot3StatsSymbolErrors);
                diag_util_printf("\tdot3ControlInUnknownOpcodes : %u\n", port_cntrs.dot3ControlInUnknownOpcodes);
                diag_util_printf("\tdot3InPauseFrames : %u\n", port_cntrs.dot3InPauseFrames);
                diag_util_printf("\tdot3OutPauseFrames : %u\n", port_cntrs.dot3OutPauseFrames);
                diag_util_printf("\t------------- RMON (RX+TX) MIB -----------------------\n");
                diag_util_printf("\tetherStatsDropEvents : %u\n", port_cntrs.etherStatsDropEvents);
                diag_util_printf("\tetherStatsBroadcastPkts : %u\n", port_cntrs.etherStatsBroadcastPkts);
                diag_util_printf("\tetherStatsMulticastPkts : %u\n", port_cntrs.etherStatsMulticastPkts);
                diag_util_printf("\tetherStatsCRCAlignErrors : %u\n", port_cntrs.etherStatsCRCAlignErrors);
                diag_util_printf("\tetherStatsUndersizePkts : %u\n", port_cntrs.etherStatsUndersizePkts);
                diag_util_printf("\tetherStatsOversizePkts : %u\n", port_cntrs.etherStatsOversizePkts);
                diag_util_printf("\tetherStatsFragments : %u\n", port_cntrs.etherStatsFragments);
                diag_util_printf("\tetherStatsJabbers : %u\n", port_cntrs.etherStatsJabbers);
                diag_util_printf("\tetherStatsCollisions : %u\n", port_cntrs.etherStatsCollisions);
                diag_util_printf("\tetherStatsPkts64Octets : %u\n", port_cntrs.etherStatsPkts64Octets);
                diag_util_printf("\tetherStatsPkts65to127Octets : %u\n", port_cntrs.etherStatsPkts65to127Octets);
                diag_util_printf("\tetherStatsPkts128to255Octets : %u\n", port_cntrs.etherStatsPkts128to255Octets);
                diag_util_printf("\tetherStatsPkts256to511Octets : %u\n", port_cntrs.etherStatsPkts256to511Octets);
                diag_util_printf("\tetherStatsPkts512to1023Octets : %u\n", port_cntrs.etherStatsPkts512to1023Octets);
                diag_util_printf("\tetherStatsPkts1024to1518Octets : %u\n", port_cntrs.etherStatsPkts1024to1518Octets);
                diag_util_printf("\t------------- RMON(RX) MIB -----------\n");
                diag_util_printf("\tetherStatsDropEvents : %u\n", port_cntrs.etherStatsDropEvents);
                diag_util_printf("\tRX_etherStatsBroadcastPkts : %u\n", port_cntrs.ifInBroadcastPkts);
                diag_util_printf("\tRX_etherStatsMulticastPkts : %u\n", port_cntrs.ifInMulticastPkts);
                diag_util_printf("\tRX_etherStatsCRCAlignErrors : %u\n", port_cntrs.rxEtherStatsCRCAlignErrors);
                diag_util_printf("\tRX_etherStatsUndersizePkts : %u\n", port_cntrs.etherStatsRxUndersizePkts);
                diag_util_printf("\tRX_etherStatsOversizePkts : %u\n", port_cntrs.etherStatsRxOversizePkts);
                diag_util_printf("\tRX_etherStatsFragments : %u\n", port_cntrs.rxEtherStatsFragments);
                diag_util_printf("\tRX_etherStatsJabbers : %u\n", port_cntrs.rxEtherStatsJabbers);
                diag_util_printf("\tRX_etherStatsPkts64Octets : %u\n", port_cntrs.etherStatsRxPkts64Octets);
                diag_util_printf("\tRX_etherStatsPkts65to127Octets : %u\n", port_cntrs.etherStatsRxPkts65to127Octets);
                diag_util_printf("\tRX_etherStatsPkts128to255Octets : %u\n", port_cntrs.etherStatsRxPkts128to255Octets);
                diag_util_printf("\tRX_etherStatsPkts256to511Octets : %u\n", port_cntrs.etherStatsRxPkts256to511Octets);
                diag_util_printf("\tRX_etherStatsPkts512to1023Octets : %u\n", port_cntrs.etherStatsRxPkts512to1023Octets);
                diag_util_printf("\tRX_etherStatsPkts1024to1518Octets : %u\n", port_cntrs.etherStatsRxPkts1024to1518Octets);
                diag_util_printf("\t------------- RMON(TX) MIB -----------\n");
                diag_util_printf("\tTX_etherStatsBroadcastPkts : %u\n", port_cntrs.etherStatsTxBroadcastPkts);
                diag_util_printf("\tTX_etherStatsMulticastPkts : %u\n", port_cntrs.etherStatsTxMulticastPkts);
                diag_util_printf("\tTX_etherStatsUndersizePkts : %u\n", port_cntrs.etherStatsTxUndersizePkts);
                diag_util_printf("\tTX_etherStatsCRCAlignErrors : %u\n", port_cntrs.txEtherStatsCRCAlignErrors);
                diag_util_printf("\tTX_etherStatsOversizePkts : %u\n", port_cntrs.etherStatsTxOversizePkts);
                diag_util_printf("\tTX_etherStatsFragments : %u\n", port_cntrs.txEtherStatsFragments);
                diag_util_printf("\tTX_etherStatsJabbers : %u\n", port_cntrs.txEtherStatsJabbers);
                diag_util_printf("\tTX_etherStatsCollisions : %u\n", port_cntrs.etherStatsCollisions);
                diag_util_printf("\tTX_etherStatsPkts64Octets : %u\n", port_cntrs.etherStatsTxPkts64Octets);
                diag_util_printf("\tTX_etherStatsPkts65to127Octets : %u\n", port_cntrs.etherStatsTxPkts65to127Octets);
                diag_util_printf("\tTX_etherStatsPkts128to255Octets : %u\n", port_cntrs.etherStatsTxPkts128to255Octets);
                diag_util_printf("\tTX_etherStatsPkts256to511Octets : %u\n", port_cntrs.etherStatsTxPkts256to511Octets);
                diag_util_printf("\tTX_etherStatsPkts512to1023Octets : %u\n", port_cntrs.etherStatsTxPkts512to1023Octets);
                diag_util_printf("\tTX_etherStatsPkts1024to1518Octets : %u\n", port_cntrs.etherStatsTxPkts1024to1518Octets);
                diag_util_printf("\t------------- Private MIB -----------\n");
                diag_util_printf("\tRX_etherStatsUndersizeDropPktsRT : %u\n", port_cntrs.etherStatsRxUndersizeDropPkts);
                diag_util_printf("\tRX_etherStatsPkts1519toMaxOctetsRT : %u\n", port_cntrs.etherStatsRxPkts1519toMaxOctets);
                diag_util_printf("\tTX_etherStatsPkts1519toMaxOctetsRT : %u\n", port_cntrs.etherStatsTxPkts1519toMaxOctets);
                diag_util_printf("\tRX_etherStatsPktsOverMaxOctetsRT : %u\n", port_cntrs.etherStatsRxPktsOverMaxOctets);
                diag_util_printf("\tTX_etherStatsPktsOverMaxOctetsRT : %u\n", port_cntrs.etherStatsTxPktsOverMaxOctets);
                diag_util_printf("\tRX_etherStatsPktsFlexibleOctetsSet0RT (%u~%u) : %u\n", range0.len_min, range0.len_max, port_cntrs.rxEtherStatsPktsFlexibleOctetsSet0RT);
                diag_util_printf("\tTX_etherStatsPktsFlexibleOctetsSet0RT (%u~%u) : %u\n", range0.len_min, range0.len_max, port_cntrs.txEtherStatsPktsFlexibleOctetsSet0RT);
                diag_util_printf("\tRX_etherStatsPktsFlexibleOctetsSet1RT (%u~%u) : %u\n", range1.len_min, range1.len_max, port_cntrs.rxEtherStatsPktsFlexibleOctetsSet1RT);
                diag_util_printf("\tTX_etherStatsPktsFlexibleOctetsSet1RT (%u~%u) : %u\n", range1.len_min, range1.len_max, port_cntrs.txEtherStatsPktsFlexibleOctetsSet1RT);
                diag_util_printf("\tRX_etherStatsPktsFlexibleOctetsCRCSet0RT (%u~%u) : %u\n", range0.len_min, range0.len_max, port_cntrs.rxEtherStatsPktsFlexibleOctetsCRCSet0RT);
                diag_util_printf("\tTX_etherStatsPktsFlexibleOctetsCRCSet0RT (%u~%u) : %u\n", range0.len_min, range0.len_max, port_cntrs.txetherStatsPktsFlexibleOctetsCRCSet0RT);
                diag_util_printf("\tRX_etherStatsPktsFlexibleOctetsCRCSet1RT (%u~%u) : %u\n", range1.len_min, range1.len_max, port_cntrs.rxEtherStatsPktsFlexibleOctetsCRCSet1RT);
                diag_util_printf("\tTX_etherStatsPktsFlexibleOctetsCRCSet1RT (%u~%u) : %u\n", range1.len_min, range1.len_max, port_cntrs.txetherStatsPktsFlexibleOctetsCRCSet1RT);
                diag_util_printf("\tRX_lengthFieldErrorRT : %u\n", port_cntrs.rxLengthFieldError);
                diag_util_printf("\tRX_falseCarrierTimesRT : %u\n", port_cntrs.rxFalseCarrierTimes);
                diag_util_printf("\tRX_underSizeOctetsRT : %u\n", port_cntrs.rxUnderSizeOctets);
                diag_util_printf("\tRX_FramingErrorsRT : %u\n", port_cntrs.rxFramingErrors);
                diag_util_printf("\tRX_MacIPGShortDropRT : %u\n", port_cntrs.rxMacIPGShortDrop);
                diag_util_printf("\tRX_MacDiscardsRT : %u\n", port_cntrs.rxMacDiscards);

                if (port == 28)
                    qNum = devInfo.capacityInfo.max_num_of_cpuQueue;
                else if (port >=24 && port < 28)
                    qNum = devInfo.capacityInfo.max_num_of_stackQueue;
                else
                    qNum = devInfo.capacityInfo.max_num_of_queue;

                for (qid = 0; qid < qNum; qid++)
                {
                    diag_util_printf("\tTX_egressQueue%dDropPktsRT : %u\n", qid, port_cntrs.egrQueueDropPkts[qid]);
                }
            }
#endif
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_MIB_DUMP_COUNTER
/*
 * mib dump counter
 */
cparser_result_t cparser_cmd_mib_dump_counter(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_stat_global_cntr_t global_cntrs;

    DIAG_UTIL_FUNC_INIT(unit);

    osal_memset(&global_cntrs, 0, sizeof(rtk_stat_global_cntr_t));
    DIAG_UTIL_ERR_CHK(rtk_stat_global_getAll(unit, &global_cntrs), ret);

    diag_util_printf("Global Counter\n");
    diag_util_printf("\tdot1dTpLearnedEntryDiscards : %u\n", global_cntrs.dot1dTpLearnedEntryDiscards);

    return CPARSER_OK;
}
#endif

#ifdef CMD_MIB_RESET_COUNTER_PORT_PORTS_ALL
/*
 * mib reset counter port ( <PORT_LIST:ports> | all )
 */
cparser_result_t cparser_cmd_mib_reset_counter_port_ports_all(cparser_context_t *context,
    char **ports_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_stat_port_reset(unit, port), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_MIB_RESET_COUNTER
/*
 * mib reset counter
 */
cparser_result_t cparser_cmd_mib_reset_counter(cparser_context_t *context)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_stat_global_reset(unit), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_MIB_GET_COUNTER_COUNTER_NAME
/*
 * mib get counter <UINT:counter_name>
 */
cparser_result_t cparser_cmd_mib_get_counter_counter_name(cparser_context_t *context,
    char **counter_name_ptr)
{
    uint32 unit;
    int32  ret = RT_ERR_FAILED;
    rtk_stat_global_type_t cntr_idx;
    uint64 cntr;

    DIAG_UTIL_FUNC_INIT(unit);

    if (0 == strcmp(context->parser->tokens[3].buf, "dot1dTpLearnedEntryDiscards"))
    {
        cntr_idx = DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_stat_global_get(unit, cntr_idx, &cntr), ret);
    diag_util_printf("Counter : %llu\n", cntr);

    return CPARSER_OK;
}
#endif

#ifdef CMD_MIB_GET_COUNTER_PORT_PORTS_ALL_COUNTER_NAME
/*
 * mib get counter port ( <PORT_LIST:ports> | all ) <UINT:counter_name>
 */
cparser_result_t cparser_cmd_mib_get_counter_port_ports_all_counter_name(cparser_context_t *context,
    char **ports_ptr,
    char **counter_name_ptr)
{
    uint32 unit, port;
    int32  ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
#if defined(CONFIG_SDK_RTL9310)
    rtk_stat_flexCntSet_t range0, range1;
#endif

    uint32 egrQueueDropIdx = 0;
    rtk_stat_port_type_t cntr_idx;
    uint64 cntr;

    DIAG_UTIL_FUNC_INIT(unit);

#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        DIAG_UTIL_ERR_CHK(rtk_stat_flexibleCntRange_get(unit, 0, &range0), ret);
        DIAG_UTIL_ERR_CHK(rtk_stat_flexibleCntRange_get(unit, 1, &range1), ret);
    }
#endif

    if (0 == strcmp(context->parser->tokens[5].buf, "dot1dTpPortInDiscards"))
    {
        cntr_idx = DOT1D_TP_PORT_IN_DISCARDS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifInOctets"))
    {
        cntr_idx = IF_IN_OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifHCInOctets"))
    {
        cntr_idx = IF_HC_IN_OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifInUcastPkts"))
    {
        cntr_idx = IF_IN_UCAST_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifHCInUcastPkts"))
    {
        cntr_idx = IF_HC_IN_UCAST_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifInMulticastPkts"))
    {
        cntr_idx = IF_IN_MULTICAST_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifHCInMulticastPkts"))
    {
        cntr_idx = IF_HC_IN_MULTICAST_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifInBroadcastPkts"))
    {
        cntr_idx = IF_IN_BROADCAST_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifHCInBroadcastPkts"))
    {
        cntr_idx = IF_HC_IN_BROADCAST_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifOutOctets"))
    {
        cntr_idx = IF_OUT_OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifHCOutOctets"))
    {
        cntr_idx = IF_HC_OUT_OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifOutUcastPkts"))
    {
        cntr_idx = IF_OUT_UCAST_PKTS_CNT_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifHCOutUcastPkts"))
    {
        cntr_idx = IF_HC_OUT_UCAST_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifOutMulticastPkts"))
    {
        cntr_idx = IF_OUT_MULTICAST_PKTS_CNT_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifHCOutMulticastPkts"))
    {
        cntr_idx = IF_HC_OUT_MULTICAST_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifOutBroadcastPkts"))
    {
        cntr_idx = IF_OUT_BROADCAST_PKTS_CNT_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifHCOutBroadcastPkts"))
    {
        cntr_idx = IF_HC_OUT_BROADCAST_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "ifOutDiscards"))
    {
        cntr_idx = IF_OUT_DISCARDS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "dot3StatsSingleCollisionFrames"))
    {
        cntr_idx = DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "dot3StatsMultipleCollisionFrames"))
    {
        cntr_idx = DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "dot3StatsDeferredTransmissions"))
    {
        cntr_idx = DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "dot3StatsLateCollisions"))
    {
        cntr_idx = DOT3_STATS_LATE_COLLISIONS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "dot3StatsExcessiveCollisions"))
    {
        cntr_idx = DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "dot3StatsSymbolErrors"))
    {
        cntr_idx = DOT3_STATS_SYMBOL_ERRORS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "dot3ControlInUnknownOpcodes"))
    {
        cntr_idx = DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "dot3InPauseFrames"))
    {
        cntr_idx = DOT3_IN_PAUSE_FRAMES_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "dot3OutPauseFrames"))
    {
        cntr_idx = DOT3_OUT_PAUSE_FRAMES_INDEX;
    }
    else if ((0 == strcmp(context->parser->tokens[5].buf, "etherStatsDropEvents")) ||
             (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsDropEvents")))
    {
        cntr_idx = ETHER_STATS_DROP_EVENTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsBroadcastPkts"))
    {
        cntr_idx = ETHER_STATS_BROADCAST_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsBroadcastPkts"))
    {
        cntr_idx = ETHER_STATS_TX_BROADCAST_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsMulticastPkts"))
    {
        cntr_idx = ETHER_STATS_MULTICAST_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsMulticastPkts"))
    {
        cntr_idx = ETHER_STATS_TX_MULTICAST_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsCRCAlignErrors"))
    {
        cntr_idx = ETHER_STATS_CRC_ALIGN_ERRORS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsCRCAlignErrors"))
    {
        cntr_idx = RX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsCRCAlignErrors"))
    {
        cntr_idx = TX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsUndersizePkts"))
    {
        cntr_idx = ETHER_STATS_UNDER_SIZE_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsUndersizePkts"))
    {
        cntr_idx = ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsUndersizePkts"))
    {
        cntr_idx = ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX;
    }
    else if ((0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsUndersizeDropPkts")) ||
             (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsUndersizeDropPktsRT")))
    {
        cntr_idx = ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsOversizePkts"))
    {
        cntr_idx = ETHER_STATS_OVERSIZE_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsOversizePkts"))
    {
        cntr_idx = ETHER_STATS_TX_OVERSIZE_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsOversizePkts"))
    {
        cntr_idx = ETHER_STATS_RX_OVERSIZE_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsFragments"))
    {
        cntr_idx = ETHER_STATS_FRAGMENTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsFragments"))
    {
        cntr_idx = RX_ETHER_STATS_FRAGMENTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsFragments"))
    {
        cntr_idx = TX_ETHER_STATS_FRAGMENTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsJabbers"))
    {
        cntr_idx = ETHER_STATS_JABBERS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsJabbers"))
    {
        cntr_idx = RX_ETHER_STATS_JABBERS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsJabbers"))
    {
        cntr_idx = TX_ETHER_STATS_JABBERS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsCollisions"))
    {
        cntr_idx = ETHER_STATS_COLLISIONS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsPkts64Octets"))
    {
        cntr_idx = ETHER_STATS_PKTS_64OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsPkts64Octets"))
    {
        cntr_idx = ETHER_STATS_TX_PKTS_64OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsPkts64Octets"))
    {
        cntr_idx = ETHER_STATS_RX_PKTS_64OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsPkts65to127Octets"))
    {
        cntr_idx = ETHER_STATS_PKTS_65TO127OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsPkts65to127Octets"))
    {
        cntr_idx = ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsPkts65to127Octets"))
    {
        cntr_idx = ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsPkts128to255Octets"))
    {
        cntr_idx = ETHER_STATS_PKTS_128TO255OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsPkts128to255Octets"))
    {
        cntr_idx = ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsPkts128to255Octets"))
    {
        cntr_idx = ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsPkts256to511Octets"))
    {
        cntr_idx = ETHER_STATS_PKTS_256TO511OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsPkts256to511Octets"))
    {
        cntr_idx = ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsPkts256to511Octets"))
    {
        cntr_idx = ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsPkts512to1023Octets"))
    {
        cntr_idx = ETHER_STATS_PKTS_512TO1023OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsPkts512to1023Octets"))
    {
        cntr_idx = ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsPkts512to1023Octets"))
    {
        cntr_idx = ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "etherStatsPkts1024to1518Octets"))
    {
        cntr_idx = ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsPkts1024to1518Octets"))
    {
        cntr_idx = ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsPkts1024to1518Octets"))
    {
        cntr_idx = ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsPkts1519toMaxOctets"))
    {
        cntr_idx = ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsPkts1519toMaxOctets"))
    {
        cntr_idx = ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_lengthFieldError"))
    {
        cntr_idx = RX_LENGTH_FIELD_ERROR_INDEX;
    }
    else if ((0 == strcmp(context->parser->tokens[5].buf, "RX_falseCarrierTimes")) ||
             (0 == strcmp(context->parser->tokens[5].buf, "RX_falseCarrierTimesRT")))
    {
        cntr_idx = RX_FALSE_CARRIER_TIMES_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_underSizeOctets"))
    {
        cntr_idx = RX_UNDER_SIZE_OCTETS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_FramingErrors"))
    {
        cntr_idx = RX_FRAMING_ERRORS_INDEX;
    }
    else if ((0 == strcmp(context->parser->tokens[5].buf, "RX_MacDiscards")) ||
              (0 == strcmp(context->parser->tokens[5].buf, "RX_MacDiscardsRT")))
    {
        cntr_idx = RX_MAC_DISCARDS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsPktsFlexibleOctetsSet0RT"))
    {
        cntr_idx = RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsPktsFlexibleOctetsSet0RT"))
    {
        cntr_idx = TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsPktsFlexibleOctetsSet1RT"))
    {
        cntr_idx = RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsPktsFlexibleOctetsSet1RT"))
    {
        cntr_idx = TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsPktsFlexibleOctetsCRCSet0RT"))
    {
        cntr_idx = RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsPktsFlexibleOctetsCRCSet0RT"))
    {
        cntr_idx = TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_etherStatsPktsFlexibleOctetsCRCSet1RT"))
    {
        cntr_idx = RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_etherStatsPktsFlexibleOctetsCRCSet1RT"))
    {
        cntr_idx = RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "RX_MacIPGShortDropRT"))
    {
        cntr_idx = RX_MAC_IPGSHORTDROP_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_egressQueue0SentPktsRT"))
    {
        cntr_idx = TX_QUEUE_0_OUT_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_egressQueue1SentPktsRT"))
    {
        cntr_idx = TX_QUEUE_1_OUT_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_egressQueue2SentPktsRT"))
    {
        cntr_idx = TX_QUEUE_2_OUT_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_egressQueue3SentPktsRT"))
    {
        cntr_idx = TX_QUEUE_3_OUT_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_egressQueue4SentPktsRT"))
    {
        cntr_idx = TX_QUEUE_4_OUT_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_egressQueue5SentPktsRT"))
    {
        cntr_idx = TX_QUEUE_5_OUT_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_egressQueue6SentPktsRT"))
    {
        cntr_idx = TX_QUEUE_6_OUT_PKTS_INDEX;
    }
    else if (0 == strcmp(context->parser->tokens[5].buf, "TX_egressQueue7SentPktsRT"))
    {
        cntr_idx = TX_QUEUE_7_OUT_PKTS_INDEX;
    }
    else if (TRUE == _is_egrQueueDropName(context->parser->tokens[5].buf, &egrQueueDropIdx))
    {
        cntr_idx = TX_QUEUE_0_DROP_PKTS_INDEX + egrQueueDropIdx;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_stat_port_get(unit, port, cntr_idx, &cntr), ret);
        diag_util_printf("Port %2u Counter : %llu\n", port, cntr);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_MIB_GET_TAG_LENGTH_RX_COUNTER_TX_COUNTER
/*
 * mib get tag-length ( rx-counter | tx-counter )
 */
cparser_result_t cparser_cmd_mib_get_tag_length_rx_counter_tx_counter(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_stat_tagCnt_type_t type;
    rtk_enable_t enable;

    DIAG_UTIL_FUNC_INIT(unit);

    if (0 == strcmp(context->parser->tokens[3].buf, "rx-counter"))
    {
        type = TAG_CNT_TYPE_RX;
    }
    else if (0 == strcmp(context->parser->tokens[3].buf, "tx-counter"))
    {
        type = TAG_CNT_TYPE_TX;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    if ((ret = rtk_stat_tagLenCntIncEnable_get(unit, type, &enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("%s MIB Counter Tag Length Configuration : %s\n",
        (type == TAG_CNT_TYPE_RX) ? "RX" : "TX", (enable == ENABLED) ? "Include" : "Exclude");

    return CPARSER_OK;
}
#endif

#ifdef CMD_MIB_GET_STACK_HEADER_LENGTH_RX_COUNTER_TX_COUNTER
/*
 * mib get stack-header-length ( rx-counter | tx-counter )
 */
cparser_result_t cparser_cmd_mib_get_stack_header_length_rx_counter_tx_counter(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_stat_stackHdrCnt_type_t type;
    rtk_enable_t enable;

    DIAG_UTIL_FUNC_INIT(unit);

    if (0 == strcmp(context->parser->tokens[3].buf, "rx-counter"))
    {
        type = STACK_HDR_CNT_TYPE_RX;
    }
    else if (0 == strcmp(context->parser->tokens[3].buf, "tx-counter"))
    {
        type = STACK_HDR_CNT_TYPE_TX;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    if ((ret = rtk_stat_stackHdrLenCntIncEnable_get(unit, type, &enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("%s MIB Counter Stacking Header Length Configuration : %s\n",
        (type == STACK_HDR_CNT_TYPE_RX) ? "RX" : "TX", (enable == ENABLED) ? "Include" : "Exclude");

    return CPARSER_OK;
}
#endif

#ifdef CMD_MIB_SET_TAG_LENGTH_RX_COUNTER_TX_COUNTER_EXCLUDE_INCLUDE
/*
 * mib set tag-length ( rx-counter | tx-counter ) ( exclude | include )
 */
cparser_result_t cparser_cmd_mib_set_tag_length_rx_counter_tx_counter_exclude_include(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_stat_tagCnt_type_t type;
    rtk_enable_t enable;

    DIAG_UTIL_FUNC_INIT(unit);

    if (0 == strcmp(context->parser->tokens[3].buf, "rx-counter"))
    {
        type = TAG_CNT_TYPE_RX;
    }
    else if (0 == strcmp(context->parser->tokens[3].buf, "tx-counter"))
    {
        type = TAG_CNT_TYPE_TX;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    if (0 == strcmp(context->parser->tokens[4].buf, "exclude"))
    {
        enable = DISABLED;
    }
    else if (0 == strcmp(context->parser->tokens[4].buf, "include"))
    {
        enable = ENABLED;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    if ((ret = rtk_stat_tagLenCntIncEnable_set(unit, type, enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_MIB_SET_STACK_HEADER_LENGTH_RX_COUNTER_TX_COUNTER_EXCLUDE_INCLUDE
/*
 * mib set stack-header-length ( rx-counter | tx-counter ) ( exclude | include )
 */
cparser_result_t cparser_cmd_mib_set_stack_header_length_rx_counter_tx_counter_exclude_include(cparser_context_t *context)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_stat_stackHdrCnt_type_t type;
    rtk_enable_t enable;

    DIAG_UTIL_FUNC_INIT(unit);

    if (0 == strcmp(context->parser->tokens[3].buf, "rx-counter"))
    {
        type = STACK_HDR_CNT_TYPE_RX;
    }
    else if (0 == strcmp(context->parser->tokens[3].buf, "tx-counter"))
    {
        type = STACK_HDR_CNT_TYPE_TX;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    if (0 == strcmp(context->parser->tokens[4].buf, "exclude"))
    {
        enable = DISABLED;
    }
    else if (0 == strcmp(context->parser->tokens[4].buf, "include"))
    {
        enable = ENABLED;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    if ((ret = rtk_stat_stackHdrLenCntIncEnable_set(unit, type, enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_MIB_GET_FLEXIBLE_COUNTER_INDEX_INDEX_PACKET_LENGTH
/*
 * mib get flexible-counter index <UINT:index> packet-length
 */
cparser_result_t cparser_cmd_mib_get_flexible_counter_index_index_packet_length(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_stat_flexCntSet_t range;

    DIAG_UTIL_FUNC_INIT(unit);

    if ((ret = rtk_stat_flexibleCntRange_get(unit, *index_ptr, &range)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("Flexible Counter %d Configuration\n", *index_ptr);
    diag_util_printf("\tMaximum packet length boundary : %4u (byte)\n", range.len_max);
    diag_util_printf("\tMinimum packet length boundary : %4u (byte)\n", range.len_min);

    return CPARSER_OK;
}
#endif

#ifdef CMD_MIB_SET_FLEXIBLE_COUNTER_INDEX_INDEX_PACKET_LENGTH_MAX_MIN_LENGTH
/*
 * mib set flexible-counter index <UINT:index> packet-length ( max | min ) <UINT:length>
 */
cparser_result_t cparser_cmd_mib_set_flexible_counter_index_index_packet_length_max_min_length(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *length_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_stat_flexCntSet_t range;

    DIAG_UTIL_FUNC_INIT(unit);

    if ((ret = rtk_stat_flexibleCntRange_get(unit, *index_ptr, &range)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (0 == strcmp(context->parser->tokens[6].buf, "max"))
    {
        range.len_max = *length_ptr;

    }
    else if (0 == strcmp(context->parser->tokens[6].buf, "min"))
    {
        range.len_min = *length_ptr;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    if ((ret = rtk_stat_flexibleCntRange_set(unit, *index_ptr, &range)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

