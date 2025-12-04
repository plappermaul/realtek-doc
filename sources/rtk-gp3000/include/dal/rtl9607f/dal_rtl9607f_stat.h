/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 39157 $
 * $Date: 2013-05-06 17:36:30 +0800 (Mon, 06 May 2013) $
 *
 * Purpose : Definition of Statistic API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Statistic Counter Reset
 *           (2) Statistic Counter Get
 *
 */

#ifndef __DAL_RTL9607F_STAT_H__
#define __DAL_RTL9607F_STAT_H__

/*
 * Include Files
 */
#include <rtk/stat.h>
#include <rtk/rt/rt_stat.h> 



/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

typedef struct dal_port_cntr_s
{
    uint64 ifInOctets;
    uint64 ifInUcastPkts;
    uint64 ifInMulticastPkts;
    uint64 ifInBroadcastPkts;
    uint64 ifInDiscards;
    uint64 ifOutOctets;
    uint64 ifOutDiscards;
    uint64 ifOutUcastPkts;
    uint64 ifOutMulticastPkts;
    uint64 ifOutBrocastPkts;
    uint64 dot1dBasePortDelayExceededDiscards;
    uint64 dot1dTpPortInDiscards;
    uint64 dot1dTpHcPortInDiscards;
    uint64 dot3InPauseFrames;
    uint64 dot3OutPauseFrames;
    uint64 dot3OutPauseOnFrames;
    uint64 dot3StatsAligmentErrors;
    uint64 dot3StatsFCSErrors;
    uint64 dot3StatsSingleCollisionFrames;
    uint64 dot3StatsMultipleCollisionFrames;
    uint64 dot3StatsDeferredTransmissions;
    uint64 dot3StatsLateCollisions;
    uint64 dot3StatsExcessiveCollisions;
    uint64 dot3StatsFrameTooLongs;
    uint64 dot3StatsSymbolErrors;
    uint64 dot3ControlInUnknownOpcodes;
    uint64 etherStatsDropEvents;
    uint64 etherStatsOctets;
    uint64 etherStatsBcastPkts;
    uint64 etherStatsMcastPkts;
    uint64 etherStatsUndersizePkts;
    uint64 etherStatsOversizePkts;
    uint64 etherStatsFragments;
    uint64 etherStatsJabbers;
    uint64 etherStatsCollisions;
    uint64 etherStatsCRCAlignErrors;
    uint64 etherStatsPkts64Octets;
    uint64 etherStatsPkts65to127Octets;
    uint64 etherStatsPkts128to255Octets;
    uint64 etherStatsPkts256to511Octets;
    uint64 etherStatsPkts512to1023Octets;
    uint64 etherStatsPkts1024to1518Octets;
    uint64 etherStatsTxOctets;
    uint64 etherStatsTxUndersizePkts;
    uint64 etherStatsTxOversizePkts;
    uint64 etherStatsTxPkts64Octets;
    uint64 etherStatsTxPkts65to127Octets;
    uint64 etherStatsTxPkts128to255Octets;
    uint64 etherStatsTxPkts256to511Octets;
    uint64 etherStatsTxPkts512to1023Octets;
    uint64 etherStatsTxPkts1024to1518Octets;
    uint64 etherStatsTxPkts1519toMaxOctets;
    uint64 etherStatsTxBcastPkts;
    uint64 etherStatsTxMcastPkts;
    uint64 etherStatsTxFragments;
    uint64 etherStatsTxJabbers;
    uint64 etherStatsTxCRCAlignErrors;
    uint64 etherStatsRxUndersizePkts;
    uint64 etherStatsRxUndersizeDropPkts;
    uint64 etherStatsRxOversizePkts;
    uint64 etherStatsRxPkts64Octets;
    uint64 etherStatsRxPkts65to127Octets;
    uint64 etherStatsRxPkts128to255Octets;
    uint64 etherStatsRxPkts256to511Octets;
    uint64 etherStatsRxPkts512to1023Octets;
    uint64 etherStatsRxPkts1024to1518Octets;
    uint64 etherStatsRxPkts1519toMaxOctets;
    uint64 inOampduPkts;
    uint64 outOampduPkts;
    uint64 ifInErrOctets;
    uint64 ifOutErrOctets;
    uint64 ifInErrors;
}dal_port_cntr_t;

/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      dal_rtl9607f_stat_init
 * Description:
 *      Initialize stat module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_PORT_CNTR_FAIL   - Could not retrieve/reset Port Counter
 * Note:
 *      Must initialize stat module before calling any stat APIs.
 */
extern int32
dal_rtl9607f_stat_init(void);

/* Function Name:
 *      dal_rtl9607f_stat_port_reset
 * Description:
 *      Reset the specified port counters in the specified device.
 * Input:
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
extern int32
dal_rtl9607f_stat_port_reset(rtk_port_t port);

/* Function Name:
 *      dal_rtl9607c_stat_port_get
 * Description:
 *      Get one specified port counter.
 * Input:
 *      port     - port id
 *      cntrIdx - specified port counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
extern int32
dal_rtl9607f_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntrIdx, uint64 *pCntr);

/* Function Name:
 *      dal_rtl9607f_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
 *      port        - port id
 * Output:
 *      pPortCntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
extern int32
dal_rtl9607f_stat_port_getAll(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs);


/* Function Name:
 *      dal_rtl9607f_stat_vlanCnt_filter_set
 * Description:
 *      Set vlan counter filter 
 * Input:
 *      index        - filter index
 *      filter       - vlan counter filer configure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT               - invalid input
 * Note:
 *      None
 */
extern int32
dal_rtl9607f_stat_vlanCnt_filter_set(uint32 index, rt_stat_vlanCnt_filter_t filter);


/* Function Name:
 *      dal_rtl9607f_stat_vlanCnt_filter_get
 * Description:
 *      Get vlan counter filter 
 * Input:
 *      index        - filter index
 * Output:
 *      pFilter       - vlan counter filer configure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT               - invalid input
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_rtl9607f_stat_vlanCnt_filter_get(uint32 index, rt_stat_vlanCnt_filter_t *pFilter);

/* Function Name:
 *      dal_rtl9607f_stat_vlanCnt_mib_get
 * Description:
 *      Get vlan counters of one specified index
 * Input:
 *      index        - vlan counter index
 * Output:
 *      pPortCntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT               - invalid index
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset vlan Counter
 * Note:
 *      None
 */
extern int32
dal_rtl9607f_stat_vlanCnt_mib_get(uint32 index, rt_stat_port_cntr_t *pPortCntrs);

/* Function Name:
 *      dal_rtl9607f_stat_vlanCnt_mib_reset
 * Description:
 *      Reset vlan counters of one specified index
 * Input:
 *      index        - vlan counter index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT               - invalid index
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset vlan Counter
 * Note:
 *      None
 */
extern int32
dal_rtl9607f_stat_vlanCnt_mib_reset(uint32 index);



#endif /* __DAL_RTL9607F_STAT_H__ */

