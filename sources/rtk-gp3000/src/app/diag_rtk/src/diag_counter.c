/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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
 * Purpose : Definition those Counter command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
//#include <dal/apollo/raw/apollo_raw_stat.h>
#include <diag_str.h>


#include <rtk/rate.h>
#include <rtk/stat.h>

int8 * _diag_counterGetReasonStr(rtk_stat_pktInfo_reasonCode_t code)
{
    static int8 reason[40];
	switch(code & 0xff)
	{
        case PKTINFO_REASON_NORMAL: osal_strcpy(reason,"normal");break;

        case PKTINFO_REASON_NAT0:
        case PKTINFO_REASON_NAT1:
        case PKTINFO_REASON_NAT2:
        case PKTINFO_REASON_NAT3:
        case PKTINFO_REASON_NAT4:
        case PKTINFO_REASON_NAT5:
        case PKTINFO_REASON_NAT6:
        case PKTINFO_REASON_NAT7:
        case PKTINFO_REASON_NAT8:
        case PKTINFO_REASON_NAT9:
        case PKTINFO_REASON_NAT10:
        case PKTINFO_REASON_NAT11:
        case PKTINFO_REASON_NAT12:
        case PKTINFO_REASON_NAT13:
        case PKTINFO_REASON_NAT14:
        case PKTINFO_REASON_NAT15:
        case PKTINFO_REASON_NAT16:
        case PKTINFO_REASON_NAT17:
        case PKTINFO_REASON_NAT18:
        case PKTINFO_REASON_NAT19:
        case PKTINFO_REASON_NAT20:
        case PKTINFO_REASON_NAT21:
        case PKTINFO_REASON_NAT22:
        case PKTINFO_REASON_NAT23:
        case PKTINFO_REASON_NAT24:
        case PKTINFO_REASON_NAT25:
        case PKTINFO_REASON_NAT26:
        case PKTINFO_REASON_NAT27:
        case PKTINFO_REASON_NAT28:
        case PKTINFO_REASON_NAT29:
        case PKTINFO_REASON_NAT30:
        case PKTINFO_REASON_NAT31:
        case PKTINFO_REASON_NAT32:
        case PKTINFO_REASON_NAT33:
        case PKTINFO_REASON_NAT34:
        case PKTINFO_REASON_NAT35:
        case PKTINFO_REASON_NAT36:
        case PKTINFO_REASON_NAT37:
        case PKTINFO_REASON_NAT38:
        case PKTINFO_REASON_NAT39:
        case PKTINFO_REASON_NAT40:
        case PKTINFO_REASON_NAT41:
        case PKTINFO_REASON_NAT42:
        case PKTINFO_REASON_NAT43:
        case PKTINFO_REASON_NAT44:
        case PKTINFO_REASON_NAT45:
        case PKTINFO_REASON_NAT46:
        case PKTINFO_REASON_NAT47:
        case PKTINFO_REASON_NAT48:
        case PKTINFO_REASON_NAT49:
        case PKTINFO_REASON_NAT50:
        case PKTINFO_REASON_NAT51:
        case PKTINFO_REASON_NAT52:
        case PKTINFO_REASON_NAT53:
        case PKTINFO_REASON_NAT54:
        case PKTINFO_REASON_NAT55:
        case PKTINFO_REASON_NAT56:
        case PKTINFO_REASON_NAT57:
        case PKTINFO_REASON_NAT58:
        case PKTINFO_REASON_NAT59:
        case PKTINFO_REASON_NAT60:
        case PKTINFO_REASON_NAT61:
        case PKTINFO_REASON_NAT62:
            osal_strcpy(reason,"NAT0");
            reason[3] += code - PKTINFO_REASON_NAT0;
            break;

        case PKTINFO_REASON_ACL_RULE0:
        case PKTINFO_REASON_ACL_RULE1:
        case PKTINFO_REASON_ACL_RULE2:
        case PKTINFO_REASON_ACL_RULE3:
        case PKTINFO_REASON_ACL_RULE4:
        case PKTINFO_REASON_ACL_RULE5:
        case PKTINFO_REASON_ACL_RULE6:
        case PKTINFO_REASON_ACL_RULE7:
        case PKTINFO_REASON_ACL_RULE8:
        case PKTINFO_REASON_ACL_RULE9:
        case PKTINFO_REASON_ACL_RULE10:
        case PKTINFO_REASON_ACL_RULE11:
        case PKTINFO_REASON_ACL_RULE12:
        case PKTINFO_REASON_ACL_RULE13:
        case PKTINFO_REASON_ACL_RULE14:
        case PKTINFO_REASON_ACL_RULE15:
        case PKTINFO_REASON_ACL_RULE16:
        case PKTINFO_REASON_ACL_RULE17:
        case PKTINFO_REASON_ACL_RULE18:
        case PKTINFO_REASON_ACL_RULE19:
        case PKTINFO_REASON_ACL_RULE20:
        case PKTINFO_REASON_ACL_RULE21:
        case PKTINFO_REASON_ACL_RULE22:
        case PKTINFO_REASON_ACL_RULE23:
        case PKTINFO_REASON_ACL_RULE24:
        case PKTINFO_REASON_ACL_RULE25:
        case PKTINFO_REASON_ACL_RULE26:
        case PKTINFO_REASON_ACL_RULE27:
        case PKTINFO_REASON_ACL_RULE28:
        case PKTINFO_REASON_ACL_RULE29:
        case PKTINFO_REASON_ACL_RULE30:
        case PKTINFO_REASON_ACL_RULE31:
        case PKTINFO_REASON_ACL_RULE32:
        case PKTINFO_REASON_ACL_RULE33:
        case PKTINFO_REASON_ACL_RULE34:
        case PKTINFO_REASON_ACL_RULE35:
        case PKTINFO_REASON_ACL_RULE36:
        case PKTINFO_REASON_ACL_RULE37:
        case PKTINFO_REASON_ACL_RULE38:
        case PKTINFO_REASON_ACL_RULE39:
        case PKTINFO_REASON_ACL_RULE40:
        case PKTINFO_REASON_ACL_RULE41:
        case PKTINFO_REASON_ACL_RULE42:
        case PKTINFO_REASON_ACL_RULE43:
        case PKTINFO_REASON_ACL_RULE44:
        case PKTINFO_REASON_ACL_RULE45:
        case PKTINFO_REASON_ACL_RULE46:
        case PKTINFO_REASON_ACL_RULE47:
        case PKTINFO_REASON_ACL_RULE48:
        case PKTINFO_REASON_ACL_RULE49:
        case PKTINFO_REASON_ACL_RULE50:
        case PKTINFO_REASON_ACL_RULE51:
        case PKTINFO_REASON_ACL_RULE52:
        case PKTINFO_REASON_ACL_RULE53:
        case PKTINFO_REASON_ACL_RULE54:
        case PKTINFO_REASON_ACL_RULE55:
        case PKTINFO_REASON_ACL_RULE56:
        case PKTINFO_REASON_ACL_RULE57:
        case PKTINFO_REASON_ACL_RULE58:
        case PKTINFO_REASON_ACL_RULE59:
        case PKTINFO_REASON_ACL_RULE60:
        case PKTINFO_REASON_ACL_RULE61:
        case PKTINFO_REASON_ACL_RULE62:
        case PKTINFO_REASON_ACL_RULE63:
             osal_strcpy(reason,"ACL0");
             reason[3] += code - PKTINFO_REASON_ACL_RULE0;
             break;

        case PKTINFO_REASON_DOS_DAEGSA: osal_strcpy(reason,"dosDaegsa");break;
        case PKTINFO_REASON_DOS_LAND_ATTACK: osal_strcpy(reason,"dosLandAattack");break;
        case PKTINFO_REASON_DOS_BLAT_ATTACK: osal_strcpy(reason,"dosBlatAttack");break;
        case PKTINFO_REASON_DOS_SYNFIN_SCAN: osal_strcpy(reason,"dosSynfinScan");break;
        case PKTINFO_REASON_DOS_XMAS_SCAN: osal_strcpy(reason,"dosXmasScan");break;
        case PKTINFO_REASON_DOS_NULL_SCAN: osal_strcpy(reason,"dosNullScan");break;
        case PKTINFO_REASON_DOS_SYN1024: osal_strcpy(reason,"dosSyn1024");break;
        case PKTINFO_REASON_DOS_TCP_SHORTHDR: osal_strcpy(reason,"dosTcpShorthdr");break;
        case PKTINFO_REASON_DOS_TCPFRAGERROR: osal_strcpy(reason,"dosTcpFragErr");break;
        case PKTINFO_REASON_DOS_ICMPFRAGMENT: osal_strcpy(reason,"dosIcmpFragment");break;
        case PKTINFO_REASON_DOS_PINGOFDEATH: osal_strcpy(reason,"dosPingOfDeath");break;
        case PKTINFO_REASON_DOS_UDPBOMB: osal_strcpy(reason,"dosUdpBomb");break;
        case PKTINFO_REASON_DOS_SYNWITHDATA: osal_strcpy(reason,"dosSynWithData");break;
        case PKTINFO_REASON_DOS_SYNFLOOD: osal_strcpy(reason,"dosSynFlood");break;
        case PKTINFO_REASON_DOS_FINFLOOD: osal_strcpy(reason,"dosFinFlood");break;
        case PKTINFO_REASON_DOS_ICMPFLOOD: osal_strcpy(reason,"dosIcmpFlood");break;

        case PKTINFO_REASON_CVLAN_POLICING: osal_strcpy(reason,"cvlanPolicing");break;
        case PKTINFO_REASON_CVLAN_EGMASK: osal_strcpy(reason,"cvlanEgmask");break;
        case PKTINFO_REASON_CVLAN_IGDROP: osal_strcpy(reason,"cvlanIgDrop");break;
        case PKTINFO_REASON_CVLAN_TYPECHECK: osal_strcpy(reason,"cvlanTypeCheck");break;

        case PKTINFO_REASON_SVLAN_UNTAG: osal_strcpy(reason,"svlanUntag");break;
        case PKTINFO_REASON_SVLAN_UNMATCH: osal_strcpy(reason,"svlanUnmath");break;
        case PKTINFO_REASON_SVLAN_DROP: osal_strcpy(reason,"svlanDrop");break;
        case PKTINFO_REASON_SVLAN_EGMASK: osal_strcpy(reason,"svlanEgMask");break;

        case PKTINFO_REASON_RLPP: osal_strcpy(reason,"rlpp");break;
        case PKTINFO_REASON_RLDP: osal_strcpy(reason,"rldp");break;
        case PKTINFO_REASON_LLDP: osal_strcpy(reason,"lldp");break;
        case PKTINFO_REASON_OTHER_RLDP: osal_strcpy(reason,"otherRldp");break;
        case PKTINFO_REASON_FORCE: osal_strcpy(reason,"force/dsl trap");break;
        case PKTINFO_REASON_PKTLEN: osal_strcpy(reason,"pktLen");break;

        case PKTINFO_REASON_SPANTREE_TX: osal_strcpy(reason,"spantreeTx");break;
        case PKTINFO_REASON_SPANTREE_RX: osal_strcpy(reason,"spantreeRx");break;

        case PKTINFO_REASON_RMA_IEEE_00: osal_strcpy(reason,"rmaIeee00");break;
        case PKTINFO_REASON_RMA_IEEE_01: osal_strcpy(reason,"rmaIeee01");break;
        case PKTINFO_REASON_RMA_IEEE_02: osal_strcpy(reason,"rmaIeee02");break;
        case PKTINFO_REASON_RMA_IEEE_03: osal_strcpy(reason,"rmaIeee03");break;
        case PKTINFO_REASON_RMA_IEEE_04: osal_strcpy(reason,"rmaIeee04");break;
        case PKTINFO_REASON_RMA_IEEE_08: osal_strcpy(reason,"rmaIeee08");break;
        case PKTINFO_REASON_RMA_IEEE_0D: osal_strcpy(reason,"rmaIeee0d");break;
        case PKTINFO_REASON_RMA_IEEE_0E: osal_strcpy(reason,"rmaIeee0e");break;
        case PKTINFO_REASON_RMA_IEEE_10: osal_strcpy(reason,"rmaIeee10");break;
        case PKTINFO_REASON_RMA_IEEE_11: osal_strcpy(reason,"rmaIeee11");break;
        case PKTINFO_REASON_RMA_IEEE_12: osal_strcpy(reason,"rmaIeee12");break;
        case PKTINFO_REASON_RMA_IEEE_13: osal_strcpy(reason,"rmaIeee13");break;
        case PKTINFO_REASON_RMA_IEEE_18: osal_strcpy(reason,"rmaIeee18");break;
        case PKTINFO_REASON_RMA_IEEE_1A: osal_strcpy(reason,"rmaIeee1a");break;
        case PKTINFO_REASON_RMA_IEEE_20: osal_strcpy(reason,"rmaIeee20");break;
        case PKTINFO_REASON_RMA_IEEE_21: osal_strcpy(reason,"rmaIeee21");break;
        case PKTINFO_REASON_RMA_IEEE_22: osal_strcpy(reason,"rmaIeee22");break;
        case PKTINFO_REASON_RMA_CISCO_CC: osal_strcpy(reason,"rmaCiscoCc");break;
        case PKTINFO_REASON_RMA_CISCO_CD: osal_strcpy(reason,"rmaCiscoCd");break;

        case PKTINFO_REASON_DROP_BY_EXTENSION_PORT: osal_strcpy(reason,"dropByExtension");break;

        case PKTINFO_REASON_L2_LEARNING_LIMIT_PORT: osal_strcpy(reason,"l2LearnLimitPerPort");break;
        case PKTINFO_REASON_L2_LEARNING_LIMIT_SYSTEM: osal_strcpy(reason,"l2LearnLimitSystem");break;

        case PKTINFO_REASON_8021X_TRAP_DROP: osal_strcpy(reason,"802.1xTrapDrop");break;
        case PKTINFO_REASON_8021X_EGRESS_PM: osal_strcpy(reason,"802.1xEgressPm");break;

        case PKTINFO_REASON_UNKN_SA: osal_strcpy(reason,"unknSa");break;
        case PKTINFO_REASON_UNKN_MA_SA: osal_strcpy(reason,"unknMaSa");break;
        case PKTINFO_REASON_LINK: osal_strcpy(reason,"link");break;
        case PKTINFO_REASON_PORT_ISOLATION: osal_strcpy(reason,"portIsolation");break;

        case PKTINFO_REASON_STORM_BCAST: osal_strcpy(reason,"stormBcast");break;
        case PKTINFO_REASON_STORM_KN_MCAST: osal_strcpy(reason,"stormKnMcast");break;
        case PKTINFO_REASON_STORM_UNKN_UCAST: osal_strcpy(reason,"stormUnknUcast");break;
        case PKTINFO_REASON_STORM_UNKN_MCAST: osal_strcpy(reason,"stormUnknMcast");break;

        case PKTINFO_REASON_UNKN_UC_DA: osal_strcpy(reason,"unknUcDa");break;
        case PKTINFO_REASON_UNKN_L2MC_DA: osal_strcpy(reason,"unknL2McDa");break;
        case PKTINFO_REASON_UNKN_IPV4MC_DA: osal_strcpy(reason,"unknIpv4McDa");break;
        case PKTINFO_REASON_UNKN_IPV6MC_DA: osal_strcpy(reason,"unknIpv6McDa");break;


        case PKTINFO_REASON_MPCP: osal_strcpy(reason,"mpcp");break;
        case PKTINFO_REASON_DS_OMCI: osal_strcpy(reason,"dsOmci");break;
        case PKTINFO_REASON_CF: osal_strcpy(reason,"classification");break;

        case PKTINFO_REASON_OAM: osal_strcpy(reason,"oam");break;
        case PKTINFO_REASON_SA_BLOCK: osal_strcpy(reason,"saBlock");break;
        case PKTINFO_REASON_DA_BLOCK: osal_strcpy(reason,"daBlock");break;

        case PKTINFO_REASON_FLOOD: osal_strcpy(reason,"flood");break;
        case PKTINFO_REASON_IGMP: osal_strcpy(reason,"igmp");break;
        case PKTINFO_REASON_MC_DATA: osal_strcpy(reason,"mcData");break;
        case PKTINFO_REASON_L34_MC_DATA: osal_strcpy(reason,"l34McData");break;
        case PKTINFO_REASON_MIRR_ISO: osal_strcpy(reason,"mirrIso");break;
        case PKTINFO_REASON_EGRESS_DROP: osal_strcpy(reason,"egressDrop");break;
        case PKTINFO_REASON_SRC_BLK: osal_strcpy(reason,"srcBlk");break;
        case PKTINFO_REASON_TX_MIRROR: osal_strcpy(reason,"txMirror");break;
        case PKTINFO_REASON_RX_MIRROR: osal_strcpy(reason,"rxMirror");break;
        case PKTINFO_REASON_L2_FWD: osal_strcpy(reason,"l2Forward");break;
        case PKTINFO_REASON_MTU_EXCEED_BIND_L2: osal_strcpy(reason,"mtuExceedBindL2");break;
        case PKTINFO_REASON_MTU_EXCEED_IPMC_ROUTE_BRIDGE: osal_strcpy(reason,"mtuExceedIpmcRouteBridge");break;
        case PKTINFO_REASON_WAN_DROP: osal_strcpy(reason,"wanDrop");break;

        case PKTINFO_REASON_PTP_TRAP: osal_strcpy(reason,"ptpTrap");break;
        case PKTINFO_REASON_PTP_RX_MIRROR: osal_strcpy(reason,"ptpRxMirror");break;
        case PKTINFO_REASON_PTP_LATCH_AND_TX_MIRROR_TO_CPU: osal_strcpy(reason,"ptpLatchAndTxMirrorToCpu");break;
        case PKTINFO_REASON_PTP: osal_strcpy(reason,"ptp");break;

        default: osal_strcpy(reason,"Unknow reason code!");break;
	}
    return reason;
}

#if defined(CONFIG_LUNA_G3_SERIES)
int32
get_dump_all_counter(rtk_port_t port,rtk_enable_t nonZero)
{
    int32 ret = RT_ERR_FAILED;
    rtk_stat_port_cntr_t portCntrs;
    rtk_stat_port_type_t type;
    uint64 cntr;

    if ((ret = rtk_stat_port_getAll(port,&portCntrs)) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }
    for (type = 0; type < MIB_PORT_CNTR_END; type++)
    {
        cntr = 0;
        switch(type)
        {
            case DOT1D_TP_PORT_IN_DISCARDS_INDEX:
                cntr = portCntrs.ifInDiscards;
                break;
            case IF_IN_OCTETS_INDEX:
                cntr = portCntrs.ifInOctets;
                break;
            case IF_IN_UCAST_PKTS_INDEX:
                cntr = portCntrs.ifInUcastPkts;
                break;
            case IF_IN_MULTICAST_PKTS_INDEX:
                cntr = portCntrs.ifInMulticastPkts;
                break;
            case IF_IN_BROADCAST_PKTS_INDEX:
                cntr = portCntrs.ifInBroadcastPkts;
                break;
            case IF_OUT_OCTETS_INDEX:
                cntr = portCntrs.ifOutOctets;
                break;
            case IF_OUT_UCAST_PKTS_CNT_INDEX:
                cntr = portCntrs.ifOutUcastPkts;
                break;
            case IF_OUT_MULTICAST_PKTS_CNT_INDEX:
                cntr = portCntrs.ifOutMulticastPkts;
                break;
            case IF_OUT_BROADCAST_PKTS_CNT_INDEX:
                cntr = portCntrs.ifOutBrocastPkts;
                break;
            case IF_OUT_DISCARDS_INDEX:
                cntr = portCntrs.ifOutDiscards;
                break;
            case DOT3_STATS_FCS_ERRORS_INDEX:
                cntr = portCntrs.dot3StatsFCSErrors;
                break;
            case DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX:
                cntr = portCntrs.dot3StatsSingleCollisionFrames;
                break;
            case DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX:
                cntr = portCntrs.dot3StatsMultipleCollisionFrames;
                break;
            case DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX:
                cntr = portCntrs.dot3StatsDeferredTransmissions;
                break;
            case DOT3_STATS_LATE_COLLISIONS_INDEX:
                cntr = portCntrs.dot3StatsLateCollisions;
                break;
            case DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX:
                cntr = portCntrs.dot3StatsExcessiveCollisions;
                break;
            case DOT3_STATS_SYMBOL_ERRORS_INDEX:
                cntr = portCntrs.dot3StatsSymbolErrors;
                break;
            case DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX:
                cntr = portCntrs.dot3ControlInUnknownOpcodes;
                break;
            case DOT3_IN_PAUSE_FRAMES_INDEX:
                cntr = portCntrs.dot3InPauseFrames;
                break;
            case DOT3_OUT_PAUSE_FRAMES_INDEX:
                cntr = portCntrs.dot3OutPauseFrames;
                break;
            case ETHER_STATS_DROP_EVENTS_INDEX:
                cntr = portCntrs.etherStatsDropEvents;
                break;
            case ETHER_STATS_TX_BROADCAST_PKTS_INDEX:
                cntr = portCntrs.etherStatsTxBcastPkts;
                break;
            case ETHER_STATS_TX_MULTICAST_PKTS_INDEX:
                cntr = portCntrs.etherStatsTxMcastPkts;
                break;
            case ETHER_STATS_CRC_ALIGN_ERRORS_INDEX:
                cntr = portCntrs.etherStatsCRCAlignErrors;
                break;
            case ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX:
                cntr = portCntrs.etherStatsTxUndersizePkts;
                break;
            case ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX:
                cntr = portCntrs.etherStatsRxUndersizePkts;
                break;
            case ETHER_STATS_TX_OVERSIZE_PKTS_INDEX:
                cntr = portCntrs.etherStatsTxOversizePkts;
                break;
            case ETHER_STATS_RX_OVERSIZE_PKTS_INDEX:
                cntr = portCntrs.etherStatsRxOversizePkts;
                break;
            case ETHER_STATS_FRAGMENTS_INDEX:
                cntr = portCntrs.etherStatsFragments;
                break;
            case ETHER_STATS_JABBERS_INDEX:
                cntr = portCntrs.etherStatsJabbers;
                break;
            case ETHER_STATS_COLLISIONS_INDEX:
                cntr = portCntrs.etherStatsCollisions;
                break;
            case ETHER_STATS_TX_PKTS_64OCTETS_INDEX:
                cntr = portCntrs.etherStatsTxPkts64Octets;
                break;
            case ETHER_STATS_RX_PKTS_64OCTETS_INDEX:
                cntr = portCntrs.etherStatsRxPkts64Octets;
                break;
            case ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX:
                cntr = portCntrs.etherStatsTxPkts65to127Octets;
                break;
            case ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX:
                cntr = portCntrs.etherStatsRxPkts65to127Octets;
                break;
            case ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX:
                cntr = portCntrs.etherStatsTxPkts128to255Octets;
                break;
            case ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX:
                cntr = portCntrs.etherStatsRxPkts128to255Octets;
                break;
            case ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX:
                cntr = portCntrs.etherStatsTxPkts256to511Octets;
                break;
            case ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX:
                cntr = portCntrs.etherStatsRxPkts256to511Octets;
                break;
            case ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX:
                cntr = portCntrs.etherStatsTxPkts512to1023Octets;
                break;
            case ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX:
                cntr = portCntrs.etherStatsRxPkts512to1023Octets;
                break;
            case ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX:
                cntr = portCntrs.etherStatsTxPkts1024to1518Octets;
                break;
            case ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX:
                cntr = portCntrs.etherStatsRxPkts1024to1518Octets;
                break;
            case ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX:
                cntr = portCntrs.etherStatsTxPkts1519toMaxOctets;
                break;
            case ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX:
                cntr = portCntrs.etherStatsRxPkts1519toMaxOctets;
                break;
            case IN_OAM_PDU_PKTS_INDEX:
                cntr = portCntrs.inOampduPkts;
                break;
            case OUT_OAM_PDU_PKTS_INDEX:
                cntr = portCntrs.outOampduPkts;
                break;
        }
        if((nonZero == ENABLED) && (cntr == 0))
            continue;

        diag_util_printf("%-35s: ",diagStr_mibName[type]);
#if defined(NO_MIB_SEPARATE)
        diag_util_printf("%25llu\n", cntr);
#else
        diag_util_mprintf("%25llu\n", cntr);
#endif
    }
    return RT_ERR_OK;
}
#endif

/*
 * mib init
 */
cparser_result_t
cparser_cmd_mib_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_stat_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_init */

/*
 * mib dump counter dot1dTpLearnedEntryDiscards
 */
cparser_result_t
cparser_cmd_mib_dump_counter_dot1dTpLearnedEntryDiscards(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint64 cntr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_stat_global_get(DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX, &cntr), ret);
    diag_util_mprintf("%s: %llu\n", diagStr_globalMibCnt[DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX], cntr);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_dump_counter_dot1dtplearnedentrydiscards */

/*
 * mib dump counter port ( <PORT_LIST:ports> | all | swpbo )
 */
cparser_result_t
cparser_cmd_mib_dump_counter_port_ports_all_swpbo(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint64 cntr;
    rtk_stat_port_type_t type;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port: %d\n", port);
#if defined(CONFIG_LUNA_G3_SERIES)
        get_dump_all_counter(port,DISABLED);
#else
        for (type = 0; type < MIB_PORT_CNTR_END; type++)
        {
            if ((ret = rtk_stat_port_get(port, type, &cntr)) == RT_ERR_OK)
            {
                diag_util_printf("%-35s: ",diagStr_mibName[type]);
#if defined(NO_MIB_SEPARATE)
                diag_util_printf("%25llu\n", cntr);
#else
                diag_util_mprintf("%25llu\n", cntr);
#endif
            }
        }
#endif
        diag_util_mprintf("\n");
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_mib_dump_counter_port_ports_all_swpbo */

/*
 * mib dump counter port ( <PORT_LIST:ports> | all | swpbo ) ( dot1dTpPortInDiscards | dot3ControlInUnknownOpcodes | dot3InPauseFrames | dot3OutPauseFrames | dot3StatsDeferredTransmissions | dot3StatsExcessiveCollisions | dot3StatsLateCollisions | dot3StatsMultipleCollisionFrames | dot3StatsSingleCollisionFrames | dot3StatsSymbolErrors )
 */
cparser_result_t
cparser_cmd_mib_dump_counter_port_ports_all_swpbo_dot1dTpPortInDiscards_dot3ControlInUnknownOpcodes_dot3InPauseFrames_dot3OutPauseFrames_dot3StatsDeferredTransmissions_dot3StatsExcessiveCollisions_dot3StatsLateCollisions_dot3StatsMultipleCollisionFrames_dot3StatsSingleCollisionFrames_dot3StatsSymbolErrors(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint64 cntr;
    rtk_stat_port_type_t type;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port: %d\n", port);
        if ('1' == TOKEN_CHAR(5,3))
        {
            type = DOT1D_TP_PORT_IN_DISCARDS_INDEX;
        }
        else if ('C' == TOKEN_CHAR(5,4))
        {
            type = DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX;
        }
        else if ('I' == TOKEN_CHAR(5,4))
        {
            type = DOT3_IN_PAUSE_FRAMES_INDEX;
        }
        else if ('O' == TOKEN_CHAR(5,4))
        {
            type = DOT3_OUT_PAUSE_FRAMES_INDEX;
        }
        else if ('D' == TOKEN_CHAR(5,9))
        {
            type = DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX;
        }
        else if ('E' == TOKEN_CHAR(5,9))
        {
            type = DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX;
        }
        else if ('L' == TOKEN_CHAR(5,9))
        {
            type = DOT3_STATS_LATE_COLLISIONS_INDEX;
        }
        else if ('M' == TOKEN_CHAR(5,9))
        {
            type = DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX;
        }
        else if ('i' == TOKEN_CHAR(5,10))
        {
            type = DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX;
        }
        else if ('y' == TOKEN_CHAR(5,10))
        {
            type = DOT3_STATS_SYMBOL_ERRORS_INDEX;
        }
        else
            return CPARSER_ERR_INVALID_PARAMS;

        DIAG_UTIL_ERR_CHK(rtk_stat_port_get(port, type, &cntr), ret);
        diag_util_mprintf("%s: %llu\n", diagStr_mibName[type], cntr);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_mib_dump_counter_port_ports_all_swpbo_dot1dtpportindiscards_dot3ControlInUnknownOpcodes_dot3inpauseframes_dot3outpauseframes_dot3statsdeferredtransmissions_dot3statsexcessivecollisions_dot3statslatecollisions_dot3statsmultiplecollisionframes_dot3statssinglecollisionframes_dot3statssymbolerrors */

/*
 * mib dump counter port ( <PORT_LIST:ports> | all | swpbo ) ( etherStatsCRCAlignErrors | etherStatsCollisions | etherStatsDropEvents | etherStatsFragments | etherStatsJabbers | etherStatsUndersizeDropPkts )
 */
cparser_result_t
cparser_cmd_mib_dump_counter_port_ports_all_swpbo_etherStatsCRCAlignErrors_etherStatsCollisions_etherStatsDropEvents_etherStatsFragments_etherStatsJabbers(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint64 cntr;
    rtk_stat_port_type_t type;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port: %d\n", port);
        if ('D' == TOKEN_CHAR(5,10))
        {
            type = ETHER_STATS_DROP_EVENTS_INDEX;
        }
        else if ('F' == TOKEN_CHAR(5,10))
        {
            type = ETHER_STATS_FRAGMENTS_INDEX;
        }
        else if ('J' == TOKEN_CHAR(5,10))
        {
            type = ETHER_STATS_JABBERS_INDEX;
        }
        else if ('U' == TOKEN_CHAR(5,10))
        {
            type = ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX;
        }
        else if ('R' == TOKEN_CHAR(5,11))
        {
            type = ETHER_STATS_CRC_ALIGN_ERRORS_INDEX;
        }
        else if ('o' == TOKEN_CHAR(5,11))
        {
            type = ETHER_STATS_COLLISIONS_INDEX;
        }
        else
            return CPARSER_ERR_INVALID_PARAMS;

        DIAG_UTIL_ERR_CHK(rtk_stat_port_get(port, type, &cntr), ret);
        diag_util_mprintf("%s: %llu\n", diagStr_mibName[type], cntr);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_mib_dump_counter_port_ports_all_swpbo_etherstatscrcalignerrors_etherstatscollisions_etherstatsdropevents_etherstatsfragments_etherstatsjabbers_etherstatsundersizedroppkts */

/*
 * mib dump counter port ( <PORT_LIST:ports> | all | swpbo ) ( etherStatsPkts64Octets | etherStatsPkts65to127Octets | etherStatsPkts128to255Octets | etherStatsPkts256to511Octets | etherStatsPkts512to1023Octets | etherStatsPkts1024to1518Octets | etherStatsPkts1519toMaxOctets | etherStatsOversizePkts | etherStatsUndersizePkts ) ( rx | tx )
 */
cparser_result_t
cparser_cmd_mib_dump_counter_port_ports_all_swpbo_etherStatsPkts64Octets_etherStatsPkts65to127Octets_etherStatsPkts128to255Octets_etherStatsPkts256to511Octets_etherStatsPkts512to1023Octets_etherStatsPkts1024to1518Octets_etherStatsPkts1519toMaxOctets_etherStatsOversizePkts_etherStatsUndersizePkts_etherStatsUndersizeDropPkts_rx_tx(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint64 cntr;
    rtk_stat_port_type_t type;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port: %d\n", port);
        if ('t' == TOKEN_CHAR(6,0))
        {
            if ('O' == TOKEN_CHAR(5,10))
            {
                type = ETHER_STATS_TX_OVERSIZE_PKTS_INDEX;
            }
            else if ('U' == TOKEN_CHAR(5,10))
            {
                type = ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX;
            }
            else if ('2' == TOKEN_CHAR(5,14))
            {
                type = ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX;
            }
            else if ('5' == TOKEN_CHAR(5,14))
            {
                type = ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX;
            }
            else if ('6' == TOKEN_CHAR(5,14))
            {
                if ('4' == TOKEN_CHAR(5,15))
                {
                    type = ETHER_STATS_TX_PKTS_64OCTETS_INDEX;
                }
                else if ('5' == TOKEN_CHAR(5,15))
                {
                    type = ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX;
                }
                else
                    return CPARSER_ERR_INVALID_PARAMS;
            }
            else if ('1' == TOKEN_CHAR(5,14))
            {
                if ('2' == TOKEN_CHAR(5,15))
                {
                    type = ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX;
                }
                else if ('0' == TOKEN_CHAR(5,15))
                {
                    type = ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX;
                }
                else if ('5' == TOKEN_CHAR(5,15))
                {
                    type = ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX;
                }
                else
                    return CPARSER_ERR_INVALID_PARAMS;
            }
            else
                return CPARSER_ERR_INVALID_PARAMS;
        }
        else if ('r' == TOKEN_CHAR(6,0))
        {
            if ('O' == TOKEN_CHAR(5,10))
            {
                type = ETHER_STATS_RX_OVERSIZE_PKTS_INDEX;
            }
            else if ('U' == TOKEN_CHAR(5,10))
            {
                type = ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX;
            }
            else if ('2' == TOKEN_CHAR(5,14))
            {
                type = ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX;
            }
            else if ('5' == TOKEN_CHAR(5,14))
            {
                type = ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX;
            }
            else if ('6' == TOKEN_CHAR(5,14))
            {
                if ('4' == TOKEN_CHAR(5,15))
                {
                    type = ETHER_STATS_RX_PKTS_64OCTETS_INDEX;
                }
                else if ('5' == TOKEN_CHAR(5,15))
                {
                    type = ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX;
                }
                else
                    return CPARSER_ERR_INVALID_PARAMS;
            }
            else if ('1' == TOKEN_CHAR(5,14))
            {
                if ('2' == TOKEN_CHAR(5,15))
                {
                    type = ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX;
                }
                else if ('0' == TOKEN_CHAR(5,15))
                {
                    type = ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX;
                }
                else if ('5' == TOKEN_CHAR(5,15))
                {
                    type = ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX;
                }
                else
                    return CPARSER_ERR_INVALID_PARAMS;
            }
            else
                return CPARSER_ERR_INVALID_PARAMS;
        }
        else
            return CPARSER_ERR_INVALID_PARAMS;

        DIAG_UTIL_ERR_CHK(rtk_stat_port_get(port, type, &cntr), ret);
        diag_util_mprintf("%s: %llu\n", diagStr_mibName[type], cntr);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_mib_dump_counter_port_ports_all_swpbo_etherstatspkts64octets_etherstatspkts65to127octets_etherstatspkts128to255octets_etherstatspkts256to511octets_etherstatspkts512to1023octets_etherstatspkts1024to1518octets_etherstatspkts1519tomaxoctets_etherstatsoversizepkts_etherstatsundersizepkts_rx_tx */

/*
 * mib dump counter port ( <PORT_LIST:ports> | all | swpbo ) ( etherStatsTxBroadcastPkts | etherStatsTxMulticastPkts | inOamPduPkts | outOamPduPkts )
 */
cparser_result_t
cparser_cmd_mib_dump_counter_port_ports_all_swpbo_etherStatsTxBroadcastPkts_etherStatsTxMulticastPkts_inOamPduPkts_outOamPduPkts(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint64 cntr;
    rtk_stat_port_type_t type;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port: %d\n", port);
        if ('i' == TOKEN_CHAR(5,0))
        {
            type = IN_OAM_PDU_PKTS_INDEX;
        }
        else if ('o' == TOKEN_CHAR(5,0))
        {
            type = OUT_OAM_PDU_PKTS_INDEX;
        }
        else if ('B' == TOKEN_CHAR(5,12))
        {
            type = ETHER_STATS_TX_BROADCAST_PKTS_INDEX;
        }
        else if ('M' == TOKEN_CHAR(5,12))
        {
            type = ETHER_STATS_TX_MULTICAST_PKTS_INDEX;
        }
        else
            return CPARSER_ERR_INVALID_PARAMS;

        DIAG_UTIL_ERR_CHK(rtk_stat_port_get(port, type, &cntr), ret);
        diag_util_mprintf("%s: %llu\n", diagStr_mibName[type], cntr);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_mib_dump_counter_port_ports_all_swpbo_etherstatstxbroadcastpkts_etherstatstxmulticastpkts_inoampdupkts_outoampdupkts */

/*
 * mib dump counter port ( <PORT_LIST:ports> | all | swpbo ) ( ifInOctets | ifInUcastPkts | ifInMulticastPkts | ifInBroadcastPkts | ifOutOctets | ifOutUcastPkts | ifOutMulticastPkts | ifOutBroadcastPkts | ifOutDiscards )
 */
cparser_result_t
cparser_cmd_mib_dump_counter_port_ports_all_swpbo_ifInOctets_ifInUcastPkts_ifInMulticastPkts_ifInBroadcastPkts_ifOutOctets_ifOutUcastPkts_ifOutMulticastPkts_ifOutBroadcastPkts_ifOutDiscards(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint64 cntr;
    rtk_stat_port_type_t type;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port: %d\n", port);
        if ('I' == TOKEN_CHAR(5,2))
        {
            if ('O' == TOKEN_CHAR(5,4))
            {
                type = IF_IN_OCTETS_INDEX;
            }
            else if ('U' == TOKEN_CHAR(5,4))
            {
                type = IF_IN_UCAST_PKTS_INDEX;
            }
            else if ('M' == TOKEN_CHAR(5,4))
            {
                type = IF_IN_MULTICAST_PKTS_INDEX;
            }
            else if ('B' == TOKEN_CHAR(5,4))
            {
                type = IF_IN_BROADCAST_PKTS_INDEX;
            }
            else
                return CPARSER_ERR_INVALID_PARAMS;
        }
        else if ('O' == TOKEN_CHAR(5,2))
        {
            if ('O' == TOKEN_CHAR(5,5))
            {
                type = IF_OUT_OCTETS_INDEX;
            }
            else if ('U' == TOKEN_CHAR(5,5))
            {
                type = IF_OUT_UCAST_PKTS_CNT_INDEX;
            }
            else if ('M' == TOKEN_CHAR(5,5))
            {
                type = IF_OUT_MULTICAST_PKTS_CNT_INDEX;
            }
            else if ('B' == TOKEN_CHAR(5,5))
            {
                type = IF_OUT_BROADCAST_PKTS_CNT_INDEX;
            }
            else if ('D' == TOKEN_CHAR(5,5))
            {
                type = IF_OUT_DISCARDS_INDEX;
            }
            else
                return CPARSER_ERR_INVALID_PARAMS;
        }
        else
            return CPARSER_ERR_INVALID_PARAMS;

        DIAG_UTIL_ERR_CHK(rtk_stat_port_get(port, type, &cntr), ret);
        diag_util_mprintf("%s: %llu\n", diagStr_mibName[type], cntr);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_mib_dump_counter_port_ports_all_swpbo_ifinoctets_ifinucastpkts_ifinmulticastpkts_ifinbroadcastpkts_ifoutoctets_ifoutucastpkts_ifoutmulticastpkts_ifoutbrocastpkts_ifoutdiscards */

/*
 * mib dump counter port ( <PORT_LIST:ports> | all | swpbo ) nonZero
 */
cparser_result_t
cparser_cmd_mib_dump_counter_port_ports_all_swpbo_nonZero(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint64 cntr;
    rtk_stat_port_type_t type;
    diag_portlist_t portlist;
    rtk_port_t port;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
#if !defined(CONFIG_LUNA_G3_SERIES)
    DIAG_UTIL_ERR_CHK(rtk_stat_global_get(DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX, &cntr), ret);
    if (cntr > 0)
    {
        diag_util_mprintf("%s: %llu\n", diagStr_globalMibCnt[DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX], cntr);
    }
#endif
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port: %d\n", port);
#if defined(CONFIG_LUNA_G3_SERIES)
        get_dump_all_counter(port,ENABLED);
#else
    	for (type = 0; type < MIB_PORT_CNTR_END; type++)
    	{
    	    if ((ret = rtk_stat_port_get(port, type, &cntr)) == RT_ERR_OK)
            {
    	        if (cntr > 0)
        		{
        			diag_util_printf("%-35s: ",diagStr_mibName[type]);
#if defined(NO_MIB_SEPARATE)
        			diag_util_printf("%25llu\n", cntr);
#else
        			diag_util_mprintf("%25llu\n", cntr);
#endif
        		}
    	    }
    	}
#endif
    	diag_util_mprintf("\n");
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_mib_dump_counter_port_ports_all_swpbo_nonzero */

/*
 * mib get count-mode
 */
cparser_result_t
cparser_cmd_mib_get_count_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_mib_count_mode_t mode;
    uint32 cnt;
    uint32 timer;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_stat_mibCntMode_get(&mode), ret);
    diag_util_mprintf("MIB count mode: %s\n", diagStr_mibLogTimer[mode]);
    if(mode == STAT_MIB_COUNT_MODE_TIMER)
    {
        DIAG_UTIL_ERR_CHK( rtk_stat_mibLatchTimer_get(&timer), ret);
        diag_util_mprintf("Timer         : %d\n", timer);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_get_count_mode */

/*
 * mib set count-mode by-timer latch-time <UINT:timer>
 */
cparser_result_t
cparser_cmd_mib_set_count_mode_by_timer_latch_time_timer(
    cparser_context_t *context,
    uint32_t  *timer_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK( rtk_stat_mibCntMode_set(STAT_MIB_COUNT_MODE_TIMER), ret);
    DIAG_UTIL_ERR_CHK( rtk_stat_mibLatchTimer_set(*timer_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_set_count_mode_by_timer_latch_time_timer */

/*
 * mib set count-mode freerun
 */
cparser_result_t
cparser_cmd_mib_set_count_mode_freerun(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK( rtk_stat_mibCntMode_set(STAT_MIB_COUNT_MODE_FREE), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_set_count_mode_freerun */

/*
 * mib get ctag-length ( rx-counter | tx-counter )
 */
cparser_result_t
cparser_cmd_mib_get_ctag_length_rx_counter_tx_counter(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_mib_tag_cnt_dir_t direction;
    rtk_mib_tag_cnt_state_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('r' == TOKEN_CHAR(3,0))
    {
        direction = STAT_MIB_TAG_CNT_DIR_RX;
    }
    else if ('t' == TOKEN_CHAR(3,0))
    {
        direction = STAT_MIB_TAG_CNT_DIR_TX;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_stat_mibCntTagLen_get(direction, &state), ret);
    diag_util_mprintf("%s: %s\n", diagStr_mibGetTagLenDir[direction], diagStr_mibGetTagLenState[state]);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_get_ctag_length_rx_counter_tx_counter */

/*
 * mib set ctag-length ( rx-counter | tx-counter ) ( exclude | include )
 */
cparser_result_t
cparser_cmd_mib_set_ctag_length_rx_counter_tx_counter_exclude_include(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_mib_tag_cnt_dir_t direction;
    rtk_mib_tag_cnt_state_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('r' == TOKEN_CHAR(3,0))
    {
        direction = STAT_MIB_TAG_CNT_DIR_RX;
    }
    else if ('t' == TOKEN_CHAR(3,0))
    {
        direction = STAT_MIB_TAG_CNT_DIR_TX;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if ('e' == TOKEN_CHAR(4,0))
    {
        state = STAT_MIB_TAG_CNT_STATE_EXCLUDE;
    }
    else if ('i' == TOKEN_CHAR(4,0))
    {
        state = STAT_MIB_TAG_CNT_STATE_INCLUDE;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;


    DIAG_UTIL_ERR_CHK(rtk_stat_mibCntTagLen_set(direction, state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_set_ctag_length_rx_counter_tx_counter_exclude_include */

/*
 * mib get reset-value
 */
cparser_result_t
cparser_cmd_mib_get_reset_value(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_mib_rst_value_t rst_value;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_stat_rstCntValue_get(&rst_value), ret);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    diag_util_mprintf("Reset MIB counter to: %s\n", diagStr_mibRstValue[rst_value]);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_get_reset_value */

/*
 * mib set reset-value ( 0 | 1 )
 */
cparser_result_t
cparser_cmd_mib_set_reset_value_0_1(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_mib_rst_value_t rst_value;

    if ('0' == TOKEN_CHAR(3,0))
    {
        rst_value = STAT_MIB_RST_TO_0;
    }
    else if ('1' == TOKEN_CHAR(3,0))
    {
        rst_value = STAT_MIB_RST_TO_1;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_stat_rstCntValue_set(rst_value), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_set_reset_value_0_1 */

/*
 * mib get sync-mode
 */
cparser_result_t
cparser_cmd_mib_get_sync_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_mib_sync_mode_t mode;
    uint32 cnt;
    uint32 timer;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_stat_mibSyncMode_get(&mode), ret);
    diag_util_mprintf("MIB sync mode: %s\n", diagStr_mibSyncMode[mode]);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_get_sync_mode */

/*
 * mib set sync-mode ( freerun | stop )
 */
cparser_result_t
cparser_cmd_mib_set_sync_mode_freerun_stop(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_mib_sync_mode_t sync_mode;

    if ('f' == TOKEN_CHAR(3,0))
    {
        sync_mode = STAT_MIB_SYNC_MODE_FREE_SYNC;
    }
    else if ('s' == TOKEN_CHAR(3,0))
    {
        sync_mode = STAT_MIB_SYNC_MODE_STOP_SYNC;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK( rtk_stat_mibSyncMode_set(sync_mode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_set_sync_mode_freerun_stop */

/*
 * mib get packet-debug-reason port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_mib_get_packet_debug_reason_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_stat_pktInfo_reasonCode_t reasonCode;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    diag_util_printf(" Port  code  Drop  Trap  Reason\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_stat_pktInfo_get(port, &reasonCode), ret);
        diag_util_mprintf(" %d     %-4u  %-4s  %-4s  %s\n",
        port,
        ((uint32)reasonCode),
        (reasonCode & 0x200) ? "V":"",
        (reasonCode & 0x100) ? "V":"",
        _diag_counterGetReasonStr(reasonCode));
    }

    return CPARSER_OK;

}    /* end of cparser_cmd_mib_get_packet_debug_reason_port_ports_all */

/*
 * mib reset counter global
 */
cparser_result_t
cparser_cmd_mib_reset_counter_global(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK( rtk_stat_global_reset(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_reset_counter_global */

/*
 * mib reset counter port ( <PORT_LIST:ports> | all | swpbo )
 */
cparser_result_t
cparser_cmd_mib_reset_counter_port_ports_all_swpbo(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_stat_port_reset(port), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_mib_reset_counter_port_ports_all_swpbo */

/*
 * mib dump statistic index <UINT:index>
 */
cparser_result_t
cparser_cmd_mib_dump_statistic_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint64 cnt;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_stat_log_get(*index_ptr, &cnt), ret);
    diag_util_mprintf("Index %d: %llu\n", *index_ptr, cnt);



    return CPARSER_OK;
}    /* end of cparser_cmd_mib_dump_statistic_index_index */

/*
 * mib reset statistic index <UINT:index>
 */
cparser_result_t
cparser_cmd_mib_reset_statistic_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_stat_log_reset(*index_ptr), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_mib_reset_statistic_index_index */

/*
 * mib set statistic mode index <UINT:index> ( bits-32 | bits-64 )
 */
cparser_result_t
cparser_cmd_mib_set_statistic_mode_index_index_bits_32_bits_64(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_stat_log_ctrl_t logCtrl;


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_stat_logCtrl_get(*index_ptr, &logCtrl), ret);

    if ('3' == TOKEN_CHAR(6,5))
    {
        logCtrl.mode = STAT_LOG_MODE_32BITS;
    }
    else if ('6' == TOKEN_CHAR(6,5))
    {
        logCtrl.mode = STAT_LOG_MODE_64BITS;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_stat_logCtrl_set(*index_ptr, logCtrl), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_mib_set_statistic_mode_index_index_bits_32_bits_64 */

/*
 * mib get statistic mode index <UINT:index>
 */
cparser_result_t
cparser_cmd_mib_get_statistic_mode_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_stat_log_ctrl_t logCtrl;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(rtk_stat_logCtrl_get(*index_ptr, &logCtrl), ret);
    diag_util_mprintf("%d: %s\n", *index_ptr, diagStr_logMibMode[logCtrl.mode]);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_get_statistic_mode_index_index */

/*
 * mib get statistic type index <UINT:index>
 */
cparser_result_t
cparser_cmd_mib_get_statistic_type_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_stat_log_ctrl_t logCtrl;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(rtk_stat_logCtrl_get(*index_ptr, &logCtrl), ret);
    diag_util_mprintf("%d: %s\n", *index_ptr, diagStr_logMibType[logCtrl.type]);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_get_statistic_type_index_index */

/*
 * mib set statistic type index <UINT:index> ( byte-count | packet-count ) */
cparser_result_t
cparser_cmd_mib_set_statistic_type_index_index_byte_count_packet_count(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_stat_log_ctrl_t logCtrl;


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_stat_logCtrl_get(*index_ptr, &logCtrl), ret);

    if ('b' == TOKEN_CHAR(6,0))
    {
        logCtrl.type = STAT_LOG_TYPE_BYTECNT;
    }
    else if ('p' == TOKEN_CHAR(6,0))
    {
        logCtrl.type = STAT_LOG_TYPE_PKTCNT;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_stat_logCtrl_set(*index_ptr, logCtrl), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_set_statistic_type_index_index_byte_count_packet_count */

/*
 * mib dump host index <UINT:index>
 */
cparser_result_t
cparser_cmd_mib_dump_host_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint64 rxCounter,txCounter;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_stat_hostCnt_get(*index_ptr, STAT_HOST_RX_OCTETS, &rxCounter), ret);
	DIAG_UTIL_ERR_CHK(rtk_stat_hostCnt_get(*index_ptr, STAT_HOST_TX_OCTETS, &txCounter), ret);

    diag_util_mprintf("Index %d: Rx:%25llu Tx:%25llu\n", *index_ptr, rxCounter, txCounter);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_dump_host_index_index */

/*
 * mib reset host index <UINT:index>
 */
cparser_result_t
cparser_cmd_mib_reset_host_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_stat_hostCnt_reset(*index_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_reset_host_index_index */

/*
 * mib get host entry <UINT:index> state
 */
cparser_result_t
cparser_cmd_mib_get_host_entry_index_state(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_enable_t state;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_stat_hostState_get(*index_ptr, &state), ret);

    diag_util_mprintf("Index %d: %s\n", *index_ptr, diagStr_enable[state]);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_get_host_entry_index_state */

/*
 * mib set host entry <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_mib_set_host_entry_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_enable_t state;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(6,0))
    {
        state = DISABLED;
    }
    else
	{
        state = ENABLED;
    }

	DIAG_UTIL_ERR_CHK(rtk_stat_hostState_set(*index_ptr, state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_set_host_entry_index_state_disable_enable */

/*
 * mib get host entry <UINT:index> mac-address
 */
cparser_result_t
cparser_cmd_mib_get_host_entry_index_mac_address(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_mac_t mac;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_rate_hostMacAddr_get(*index_ptr, &mac), ret);

    diag_util_mprintf("Index %d: MAC-Address: %s\n", *index_ptr, diag_util_inet_mactoa(&mac.octet[0]));

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_get_host_entry_index_mac_address*/

/*
 * mib set host entry <UINT:index> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_mib_set_host_entry_index_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    rtk_mac_t mac;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    osal_memcpy(&mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    DIAG_UTIL_ERR_CHK(rtk_rate_hostMacAddr_set(*index_ptr, &mac), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_mib_set_host_entry_index_mac_address_mac */

