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
 * $Revision: 97277 $
 * $Date: 2019-05-29 17:51:38 +0800 (Wed, 29 May 2019) $
 *
 * Purpose : Definition those public diagnostic routing APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Remote/Local Loopback
 *           (2) RTCT
 *           (3) Dump whole registers and tables
 */

#ifndef __RTK_DIAG_H__
#define __RTK_DIAG_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/*
 * Symbol Definition
 */
typedef enum rtk_diag_dumpType_e
{
    DUMP_TYPE_REG = 0,
    DUMP_TYPE_TABLE,
    DUMP_TYPE_END
} rtk_diag_dumpType_t;

#if (defined(CONFIG_SDK_APP_DIAG_EXT) && defined (CONFIG_SDK_RTL9300))
typedef enum rtk_diag_l2_entry_type_e {
    L2_ENTRY_UNICAST = 0,
    L2_ENTRY_MULTICAST
} rtk_diag_l2_entry_type_t;

typedef enum rtk_diag_l3_entry_type_e {
    L3_ENTRY_L2_MCAST_IPV4_SINGLE_VLAN,
    L3_ENTRY_L2_MCAST_IPV4_DOUBLE_VLAN,
    L3_ENTRY_L2_MCAST_IPV6_SINGLE_VLAN,
    L3_ENTRY_L2_MCAST_IPV6_DOUBLE_VLAN,
    L3_ENTRY_L3_UCAST_IPV4,
    L3_ENTRY_L3_UCAST_IPV6,
    L3_ENTRY_L3_MCAST_IPV4_SINGLE,
    L3_ENTRY_L3_MCAST_IPV4_DOUBLE,
    L3_ENTRY_L3_MCAST_IPV6_SINGLE,
    L3_ENTRY_L3_MCAST_IPV6_DOUBLE
} rtk_diag_l3_entry_type_t;

typedef enum rtk_diag_entry_type_e {
    ENTRY_REG = 0,
    ENTRY_TABLE,
    ENTRY_HASH
} rtk_diag_entry_type_t;

typedef enum rtk_diag_value_type_e {
    VALUE_NORMAL = 0,
    VALUE_MAC,
    VALUE_IPV6,
    VALUE_IPV4
} rtk_diag_value_type_t;
#endif

#define REVERSE_BIT_INVALID 0x0
#define REVERSE_BIT_VALID   0x1

/*
 * Data Declaration
 */

/* RTCT result Status of one channel */
typedef struct rtk_rtctChannelStatus_s
{
    uint32      channelShort;
    uint32      channelOpen;
    uint32      channelLowMismatch;
    uint32      channelHighMismatch;
    uint32      channelCrossoverA;
    uint32      channelCrossoverB;
    uint32      channelCrossoverC;
    uint32      channelCrossoverD;
    uint32      channelLen;
} rtk_rtctChannelStatus_t;

/* result of RTCT test */
typedef struct rtk_rtctResult_s
{
    rtk_port_speed_t    linkType;
    union
    {
        struct fe_result_s
        {
            uint32      isRxShort;
            uint32      isTxShort;
            uint32      isRxOpen;
            uint32      isTxOpen;
            uint32      isRxMismatch;
            uint32      isTxMismatch;
            uint32      isRxLinedriver;
            uint32      isTxLinedriver;
            uint32      rxLen;
            uint32      txLen;
        } fe_result;

        struct ge_result_s
        {
            uint32      channelAShort;
            uint32      channelBShort;
            uint32      channelCShort;
            uint32      channelDShort;

            uint32      channelAOpen;
            uint32      channelBOpen;
            uint32      channelCOpen;
            uint32      channelDOpen;

            uint32      channelAMismatch;
            uint32      channelBMismatch;
            uint32      channelCMismatch;
            uint32      channelDMismatch;

            uint32      channelALinedriver;
            uint32      channelBLinedriver;
            uint32      channelCLinedriver;
            uint32      channelDLinedriver;

            uint32      channelAHiImpedance;
            uint32      channelBHiImpedance;
            uint32      channelCHiImpedance;
            uint32      channelDHiImpedance;

            uint32      channelACross;
            uint32      channelBCross;
            uint32      channelCCross;
            uint32      channelDCross;

            uint32      channelAPartialCross;
            uint32      channelBPartialCross;
            uint32      channelCPartialCross;
            uint32      channelDPartialCross;

            /* when paire is busy, RTCT shall re-do by calling start API again. */
            uint32      channelAPairBusy;
            uint32      channelBPairBusy;
            uint32      channelCPairBusy;
            uint32      channelDPairBusy;

            uint32      channelALen;
            uint32      channelBLen;
            uint32      channelCLen;
            uint32      channelDLen;
        } ge_result;

        struct channels_result_s
        {
            rtk_rtctChannelStatus_t     a;
            rtk_rtctChannelStatus_t     b;
            rtk_rtctChannelStatus_t     c;
            rtk_rtctChannelStatus_t     d;
        } channels_result;
    } un;
} rtk_rtctResult_t;

#if (defined(CONFIG_SDK_APP_DIAG_EXT) && defined (CONFIG_SDK_RTL9300))
typedef struct rtk_diag_debug_s
{
    rtk_diag_entry_type_t type;
    int32 hash_l2;
    int32 cmp_vid;
    int32 hash_type;
    char table_reg_name[64];
    uint32 table_reg_offset;
    char field_name[64];
    uint32 field_offset;
    rtk_diag_value_type_t value_type;
    uint32 value[4];
    rtk_vlan_t vid;
    rtk_ip_addr_t sip[4];
    rtk_ip_addr_t dip[4];
    rtk_mac_t mac;
    uint32 table_index;
} rtk_diag_debug_t;
#endif

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name    : diagnostic */
/* Sub-module Name: Global     */

/* Function Name:
 *      rtk_diag_init
 * Description:
 *      Initialize diagnostic module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Module must be initialized before using all of APIs in this module
 * Changes:
 *      None
 */
extern int32
rtk_diag_init(uint32 unit);

/* Module Name    : Diag */
/* Sub-module Name: RTCT */

/* Function Name:
 *      rtk_diag_portRtctResult_get
 * Description:
 *      Get test result of Realtek Cable Tester.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT             - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D
 * Note:
 *      (1) If linkType is PORT_SPEED_1000M, test result will be stored in ge_result.
 *          If linkType is PORT_SPEED_10M or PORT_SPEED_100M, test result will be stored in fe_result.
 *      (2) The unit of cable lenght is centimeter.
 * Changes:
 *      [3.2.0]
 *          rtk_rtctResult_t adds HiImpedance, Cross, PartialCross, and PairBusy.
 */
extern int32
rtk_diag_portRtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult);

/* Function Name:
 *      rtk_diag_rtctEnable_set
 * Description:
 *      Start Realtek Cable Tester for ports.
 * Input:
 *      unit      - unit id
 *      pPortmask - the ports for RTCT test
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8208G, 8208D, 8208L, 8214C, 8214FB, 8214B, 8212B, 8218B, 8218FB, 8214FC, 8218D
 * Note:
 *      When RTCT starts, the port won't transmit and receive normal traffic.
 * Changes:
 *      None
 */
extern int32
rtk_diag_rtctEnable_set(uint32 unit, rtk_portmask_t *pPortmask);

/* Function Name:
 *      rtk_diag_table_whole_read
 * Description:
 *      Dump whole table content in console for debugging
 * Input:
 *      unit        - unit id
 *      table_index - dumped table index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - dumped table index is out of range
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_diag_table_whole_read(uint32 unit, uint32 table_index);

/* Function Name:
 *      rtk_diag_reg_whole_read
 * Description:
 *      Dump all registers' value in console for debugging
 * Input:
 *      unit - unit id
 * Output:
 *      all registers value in console.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_diag_reg_whole_read(uint32 unit);

/* Function Name:
 *      rtk_diag_peripheral_register_dump
 * Description:
 *      Dump Chip peripheral registers
 * Input:
 *      unit           - unit id
 * Output:
 *      N/A
 * Return:
 *      RT_ERR_OK        - OK
 *      RT_ERR_FAILED   - Failed
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_diag_peripheral_register_dump(uint32 unit);

/* Function Name:
 *      rtk_diag_phy_reg_whole_read
 * Description:
 *      Dump all standard registers of PHY of all ports
 * Input:
 *      unit - unit id
 * Output:
 *      all registers value in console.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_diag_phy_reg_whole_read(uint32 unit);

/* Function Name:
 *      rtk_diag_tableEntry_read
 * Description:
 *      Dump table entry of a specific range
 * Input:
 *      unit - unit id
 *      table_index - table index number
 *      start_index - entry's start index for dump
 *      end_index - entry's end index for dump
 *      detail - TRUE: display field information; False: summary information.
 * Output:
 *      Display out the information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_diag_tableEntry_read(uint32 unit, uint32 table_index, uint32 start_index, uint32 end_index, uint32 detail);

#if (defined(CONFIG_SDK_APP_DIAG_EXT) && defined (CONFIG_SDK_RTL9300))
/* Function Name:
 *      rtk_diag_table_reg_field_get
 * Description:
 *      Dump the specified registers/table field value in console for debugging
 * Input:
 *      unit - unit id
 *      pCfg - diag config
 * Output:
 *      the specified registers/table field value.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9300
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_diag_table_reg_field_get(uint32 unit, rtk_diag_debug_t *pCfg);

/* Function Name:
 *      rtk_diag_table_reg_field_set
 * Description:
 *      Dump the specified registers/table field value in console for debugging
 * Input:
 *      unit - unit id
 *      pCfg - diag config
 * Output:
 *      N/A.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9300
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_diag_table_reg_field_set(uint32 unit, rtk_diag_debug_t *pCfg);
#endif

/* Function Name:
 *      rtk_diag_regArray_get
 * Description:
 *      Get the specified register value in console for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
 *      index_1 - index of array 1
 *      index_2 - index of array 2
 * Output:
 *      pValue  - pointer of value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_regArray_get(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 *pValue);

/* Function Name:
 *      rtk_diag_regArray_set
 * Description:
 *      Set the specified register value in console for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
 *      index_1 - index of array 1
 *      index_2 - index of array 2
 *      pValue  - pointer of value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_regArray_set(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 *pValue);

/* Function Name:
 *      rtk_diag_regArrayField_get
 * Description:
 *      Get the specified register-field value in console for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
 *      index_1 - index of array 1
 *      index_2 - index of array 2
 *      field   - field id
 * Output:
 *      pValue  - pointer of value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_regArrayField_get(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 field, uint32 *pValue);

/* Function Name:
 *      rtk_diag_regArrayField_set
 * Description:
 *      Set the specified register-field value in console for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
 *      index_1 - index of array 1
 *      index_2 - index of array 2
 *      field   - field id
 *      pValue  - pointer of value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_regArrayField_set(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 field, uint32 *pValue);

/* Function Name:
 *      rtk_diag_tableEntry_get
 * Description:
 *      Get the specified table entry in console for debugging
 * Input:
 *      unit  - unit id
 *      table - table id
 *      addr  - address of entry
 * Output:
 *      pData - pointer of data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_tableEntry_get(uint32 unit, uint32 table, uint32 addr, uint32 *pData);

/* Function Name:
 *      rtk_diag_tableEntry_set
 * Description:
 *      Set the specified table entry in console for debugging
 * Input:
 *      unit  - unit id
 *      table - table id
 *      addr  - address of entry
 *      pData - pointer of data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_tableEntry_set(uint32 unit, uint32 table, uint32 addr, uint32 *pData);

/* Function Name:
 *      rtk_diag_table_index_name
 * Description:
 *      Dump whole table index mapping name
 * Input:
 *      unit        - unit id
 *      table_index - dumped table index name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - dumped table index is out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_diag_table_index_name(uint32 unit, uint32 table_index);

#endif /* __RTK_DIAG_H__ */

