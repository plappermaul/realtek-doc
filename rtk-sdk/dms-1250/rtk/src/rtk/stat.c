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
 * Purpose : Definition of Statistic API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Statistic Counter Reset
 *           (2) Statistic Counter Get
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/stat.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      rtk_stat_init
 * Description:
 *      Initialize stat module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID               - invalid unit id
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_PORT_CNTR_FAIL   - Could not retrieve/reset Port Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize stat module before calling any stat APIs.
 * Changes:
 *      None
 */
int32
rtk_stat_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_init(unit);
} /* end of rtk_stat_init */

/* Function Name:
 *      rtk_stat_enable_get
 * Description:
 *      Get the status to check whether MIB counters are enabled or not.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer buffer of enable or disable MIB counters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_stat_enable_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_enable_get(unit, pEnable);
} /* end of rtk_stat_enable_get */

/* Function Name:
 *      rtk_stat_enable_set
 * Description:
 *      Set the status to enable or disable MIB counters
 * Input:
 *      unit   - unit id
 *      enable - enable/disable MIB counters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_stat_enable_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_enable_set(unit, enable);
} /* end of rtk_stat_enable_set */

/* Function Name:
 *      rtk_stat_global_reset
 * Description:
 *      Reset the global counters in the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID               - invalid unit id
 *      RT_ERR_NOT_INIT              - The module is not initial
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_stat_global_reset(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_global_reset(unit);
} /* end of rtk_stat_global_reset */

/* Function Name:
 *      rtk_stat_port_reset
 * Description:
 *      Reset the specified port counters in the specified device.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_stat_port_reset(uint32 unit, rtk_port_t port)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_port_reset(unit, port);

} /* end of rtk_stat_port_reset */

/* Function Name:
 *      rtk_stat_global_get
 * Description:
 *      Get one specified global counter in the specified device.
 * Input:
 *      unit     - unit id
 *      cntr_idx - specified global counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - The module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL    - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported management frame is as following:
 *      rtk_stat_global_type_t \ Chip :             8390    8380    9300    9310
 *      - DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX     O       O       O       O
 * Changes:
 *      None
 */
int32
rtk_stat_global_get(uint32 unit, rtk_stat_global_type_t cntr_idx, uint64 *pCntr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_global_get(unit, cntr_idx, pCntr);
} /* end of rtk_stat_global_get */

/* Function Name:
 *      rtk_stat_global_getAll
 * Description:
 *      Get all global counters in the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      pGlobal_cntrs - pointer buffer of global counter structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - The module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_stat_global_getAll(uint32 unit, rtk_stat_global_cntr_t *pGlobal_cntrs)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_global_getAll(unit, pGlobal_cntrs);
} /* end of rtk_stat_global_getAll */

/* Function Name:
 *      rtk_stat_port_get
 * Description:
 *      Get one specified port counter in the specified device.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      cntr_idx - specified port counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE           - input parameter out of range
 *      RT_ERR_INPUT                  - Invalid input parameter
 *      RT_ERR_STAT_INVALID_PORT_CNTR - Invalid Port Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Supported management frame is as following:
 *      - rtk_stat_port_type_t \ Chip:                  8390    8380    9300    9310
 *      - IF_IN_OCTETS_INDEX                            O       O       O       O
 *      - IF_HC_IN_OCTETS_INDEX                         O       O       O       O
 *      - IF_IN_UCAST_PKTS_INDEX                        O       O       O       O
 *      - IF_HC_IN_UCAST_PKTS_INDEX                     X       X       O       O
 *      - IF_IN_MULTICAST_PKTS_INDEX                    O       O       O       O
 *      - IF_HC_IN_MULTICAST_PKTS_INDEX                 X       X       O       O
 *      - IF_IN_BROADCAST_PKTS_INDEX                    O       O       O       O
 *      - IF_HC_IN_BROADCAST_PKTS_INDEX                 X       X       O       O
 *      - IF_OUT_OCTETS_INDEX                           O       O       O       O
 *      - IF_HC_OUT_OCTETS_INDEX                        O       O       O       O
 *      - IF_OUT_UCAST_PKTS_CNT_INDEX                   O       O       O       O
 *      - IF_HC_OUT_UCAST_PKTS_INDEX                    X       X       O       O
 *      - IF_OUT_MULTICAST_PKTS_CNT_INDEX               O       O       O       O
 *      - IF_HC_OUT_MULTICAST_PKTS_INDEX                X       X       O       O
 *      - IF_OUT_BROADCAST_PKTS_CNT_INDEX               O       O       O       O
 *      - IF_HC_OUT_BROADCAST_PKTS_INDEX                X       X       O       O
 *      - IF_OUT_DISCARDS_INDEX                         O       O       O       O
 *      - DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX      O       O       O       O
 *      - DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX    O       O       O       O
 *      - DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX       O       O       O       O
 *      - DOT3_STATS_LATE_COLLISIONS_INDEX              O       O       O       O
 *      - DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX         O       O       O       O
 *      - DOT3_STATS_SYMBOL_ERRORS_INDEX                O       O       O       O
 *      - DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX         O       O       O       O
 *      - DOT3_IN_PAUSE_FRAMES_INDEX                    O       O       O       O
 *      - DOT3_OUT_PAUSE_FRAMES_INDEX                   O       O       O       O
 *      - ETHER_STATS_DROP_EVENTS_INDEX                 O       O       O       O
 *      - ETHER_STATS_BROADCAST_PKTS_INDEX              O       O       O       O
 *      - ETHER_STATS_TX_BROADCAST_PKTS_INDEX           O       O       O       O
 *      - ETHER_STATS_MULTICAST_PKTS_INDEX              O       O       O       O
 *      - ETHER_STATS_TX_MULTICAST_PKTS_INDEX           O       O       O       O
 *      - ETHER_STATS_CRC_ALIGN_ERRORS_INDEX            O       O       O       O
 *      - RX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX         X       X       O       O
 *      - TX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX         O       X       O       O
 *      - ETHER_STATS_UNDER_SIZE_PKTS_INDEX             O       O       O       O
 *      - ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX          O       O       O       O
 *      - ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX          O       O       O       O
 *      - ETHER_STATS_OVERSIZE_PKTS_INDEX               O       O       O       O
 *      - ETHER_STATS_RX_OVERSIZE_PKTS_INDEX            O       O       O       O
 *      - ETHER_STATS_TX_OVERSIZE_PKTS_INDEX            O       O       O       O
 *      - ETHER_STATS_FRAGMENTS_INDEX                   O       O       O       O
 *      - RX_ETHER_STATS_FRAGMENTS_INDEX                X       X       O       O
 *      - TX_ETHER_STATS_FRAGMENTS_INDEX                O       X       O       O
 *      - ETHER_STATS_JABBERS_INDEX                     O       O       O       O
 *      - RX_ETHER_STATS_JABBERS_INDEX                  X       X       O       O
 *      - TX_ETHER_STATS_JABBERS_INDEX                  O       X       O       O
 *      - ETHER_STATS_COLLISIONS_INDEX                  O       O       O       O
 *      - ETHER_STATS_PKTS_64OCTETS_INDEX               O       O       O       O
 *      - ETHER_STATS_RX_PKTS_64OCTETS_INDEX            O       O       O       O
 *      - ETHER_STATS_TX_PKTS_64OCTETS_INDEX            O       O       O       O
 *      - ETHER_STATS_PKTS_65TO127OCTETS_INDEX          O       O       O       O
 *      - ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX       O       O       O       O
 *      - ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX       O       O       O       O
 *      - ETHER_STATS_PKTS_128TO255OCTETS_INDEX         O       O       O       O
 *      - ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX      O       O       O       O
 *      - ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX      O       O       O       O
 *      - ETHER_STATS_PKTS_256TO511OCTETS_INDEX         O       O       O       O
 *      - ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX      O       O       O       O
 *      - ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX      O       O       O       O
 *      - ETHER_STATS_PKTS_512TO1023OCTETS_INDEX        O       O       O       O
 *      - ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX     O       O       O       O
 *      - ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX     O       O       O       O
 *      - ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX       O       O       O       O
 *      - ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX    O       O       O       O
 *      - ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX    O       O       O       O
 *      - ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX     O       O       O       O
 *      - ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX     O       O       O       O
 *      - ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX     O       O       O       O
 *      - ETHER_STATS_RX_PKTS_OVERMAXOCTETS_INDEX       X       X       O       O
 *      - ETHER_STATS_TX_PKTS_OVERMAXOCTETS_INDEX       X       X       O       O
 *      - RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX     O       O       O       O
 *      - TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET0_INDEX     O       O       O       O
 *      - RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX X       X       O       O
 *      - TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET0_INDEX X       X       O       O
 *      - RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX     O       O       O       O
 *      - TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_SET1_INDEX     O       O       O       O
 *      - RX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX X       X       O       O
 *      - TX_ETHER_STATS_PKTS_FLEXIBLE_OCTECTS_CRC_SET1_INDEX X       X       O       O
 *      - RX_LENGTH_FIELD_ERROR_INDEX                   O       X       O       O
 *      - RX_FALSE_CARRIER_TIMES_INDEX                  O       X       O       O
 *      - RX_UNDER_SIZE_OCTETS_INDEX                    O       X       O       O
 *      - RX_FRAMING_ERRORS_INDEX                       O       X       O       O
 *      - RX_PARSER_ERROR_INDEX                         X       X       O       O
 *      - RX_MAC_DISCARDS_INDEX                         O       O       O       O
 *      - DOT1D_TP_PORT_IN_DISCARDS_INDEX               O       O       O       O
 * Changes:
 *      None
 */
int32
rtk_stat_port_get(uint32 unit, rtk_port_t port, rtk_stat_port_type_t cntr_idx, uint64 *pCntr)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_port_get(unit, port, cntr_idx, pCntr);

} /* end of rtk_stat_port_get */

/* Function Name:
 *      rtk_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pPort_cntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_NULL_POINTER           - input parameter may be null pointer
 *      RT_ERR_STAT_INVALID_PORT_CNTR - Invalid Port Counter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_stat_port_getAll(uint32 unit, rtk_port_t port, rtk_stat_port_cntr_t *pPort_cntrs)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_port_getAll(unit, port, pPort_cntrs);

} /* end of rtk_stat_port_getAll */

/* Function Name:
 *      rtk_stat_tagLenCntIncEnable_get
 * Description:
 *      Get RX/TX counter to include or exclude tag length in the specified device.
 * Input:
 *      unit        - unit id
 *      tagCnt_type - specified RX counter or TX counter
 * Output:
 *      pEnable     - pointer buffer of including/excluding tag length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Inner(4B) and outer(4B) tag length can be included or excluded to the counter through the API.
 * Changes:
 *      None
 */
int32
rtk_stat_tagLenCntIncEnable_get(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_tagLenCntIncEnable_get(unit, tagCnt_type, pEnable);
} /* end of rtk_stat_tagLenCntIncEnable_get */

/* Function Name:
 *      rtk_stat_tagLenCntIncEnable_set
 * Description:
 *      Set RX/TX counter to include or exclude tag length in the specified device.
 * Input:
 *      unit        - unit id
 *      tagCnt_type - specified RX counter or TX counter
 *      enable      - include/exclude Tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Inner(4B) and outer(4B) tag length can be included or excluded to the counter through the API.
 * Changes:
 *      None
 */
int32
rtk_stat_tagLenCntIncEnable_set(uint32 unit, rtk_stat_tagCnt_type_t tagCnt_type, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_tagLenCntIncEnable_set(unit, tagCnt_type, enable);
} /* end of rtk_stat_tagLenCntIncEnable_set */

/* Function Name:
 *      rtk_stat_stackHdrLenCntIncEnable_get
 * Description:
 *      Get RX/TX counter to include or exclude tag length in the specified device.
 * Input:
 *      unit        - unit id
 *      type        - specified RX counter or TX counter
 * Output:
 *      pEnable     - pointer buffer of including/excluding tag length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      Stacking header length can be included or excluded to the counter through the API.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_stat_stackHdrLenCntIncEnable_get(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_stackHdrLenCntIncEnable_get(unit, type, pEnable);
} /* end of rtk_stat_stackHdrLenCntIncEnable_get */

/* Function Name:
 *      rtk_stat_stackHdrLenCntIncEnable_set
 * Description:
 *      Set RX/TX counter to include or exclude stacking header length in the specified device.
 * Input:
 *      unit        - unit id
 *      type        - specified RX counter or TX counter
 *      enable      - include/exclude Tag length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      Stacking header length can be included or excluded to the counter through the API.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_stat_stackHdrLenCntIncEnable_set(uint32 unit, rtk_stat_stackHdrCnt_type_t type, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_stackHdrLenCntIncEnable_set(unit, type, enable);
} /* end of rtk_stat_stackHdrLenCntIncEnable_set */

/* Function Name:
 *      rtk_stat_flexibleCntRange_get
 * Description:
 *      Get the flexible mib counter max/min boundary.
 * Input:
 *      unit        - unit id
 *      idx         - flexible mib counter set index
 * Output:
 *      pRange      - pointer buffer of the boundary value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      Per flexible counter MAX/MIN boundary value can be up to max frame length.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_stat_flexibleCntRange_get(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_flexibleCntRange_get(unit, idx, pRange);
} /* end of rtk_stat_flexibleCntRange_get */

/* Function Name:
 *      rtk_stat_flexibleCntRange_set
 * Description:
 *      Set the flexible mib counter max/min boundary.
 * Input:
 *      unit        - unit id
 *      idx         - flexible mib counter set index
 *      pRange      - pointer of the boundary value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      Per flexible counter MAX/MIN boundary value can be up to max frame length.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_stat_flexibleCntRange_set(uint32 unit, uint32 idx, rtk_stat_flexCntSet_t *pRange)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->stat_flexibleCntRange_set(unit, idx, pRange);
} /* end of rtk_stat_flexibleCntRange_set */


