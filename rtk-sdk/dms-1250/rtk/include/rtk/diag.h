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
#if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
#include <hal/chipdef/allreg.h>
#endif
#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
#include <hal/chipdef/allmem.h>
#endif
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/phy.h>

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
#define RTK_DIAG_TABLE_DATAREG_NUM_MAX 64
/*
 * Data Declaration
 */


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

#define RTK_DIAG_REG_WORD_NUM_MAX  64 // number of 4bytes

#if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
#define RTK_DIAG_REG_ARRAY_IDX_MAX 2
#define RTK_DIAG_REG_IDX_NO_FILTER -1

/* filter for REG array */
typedef struct rtk_diag_regArrFilter_s
{
    char      name[REG_NAME_LEN];
    int32     filter_1;   /* array index filter 1 */
    int32     filter_2;   /* array index filter 2 */
} rtk_diag_regArrFilter_t;

typedef struct rtk_diag_reg_info_s
{
    uint32 index;
    uint32 offset;            /* offset address */
    uint32 field_num:16;         /* total field numbers */
    uint32 lport:8;           /* port start index */
    uint32 hport:8;           /* port end index */
    uint32 larray:16;          /* array start index */
    uint32 harray:16;          /* array end index */
    uint32 reg_words;
    char name[REG_NAME_LEN];  /* name char array */
} rtk_diag_regInfo_t;

typedef struct rtk_diag_regFieldInfo_s
{
    uint32 index;              /* field enum index */
    uint16 lsp;               /* LSP (Least Significant Position) of the field */
    uint16 len;               /* field length */
    char field_name[REG_NAME_LEN];     /* name char array */
} rtk_diag_regFieldInfo_t;

#endif //CONFIG_SDK_DUMP_REG_WITH_NAME


#define RTK_DIAG_TBL_DATAREG_NUM_MAX  64 // number of 4bytes
#define RTK_DIAG_TBL_FIELD_WORDS_MAX  3  // number of 4bytes

#if defined(CONFIG_SDK_DUMP_TABLE_WITH_NAME)
typedef struct rtk_diag_tblInfo_s
{
    uint32 index;                     /* table enum index */
    char name[TBL_NAME_LEN];          /* table name */
    unsigned int type;                /* access table type */
    unsigned int size;                /* table size */
    unsigned int datareg_num;         /* total data registers */
    unsigned int field_num;           /* total field numbers */
} rtk_diag_tblInfo_t;

typedef struct rtk_diag_tblFieldInfo_s
{
    uint16 lsp;               /* LSP (Least Significant Position) of the field */
    uint16 len;               /* field length */
    char name[TBL_NAME_LEN];  /* field name */
} rtk_diag_tblFieldInfo_t;
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

/* Function Name:
 *      rtk_diag_tableEntryDatareg_write
 * Description:
 *      Write a data reg of a table entry
 * Input:
 *      unit - unit id
 *      table_index - table index number
 *      entry_index - entry's start index for dump
 *      datareg_index - data register index
 *      pData - 32-bit data to be written
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
 *      [SDK_3.6.2]
 *          New added function.
 */
extern int32
rtk_diag_tableEntryDatareg_write(uint32 unit, uint32 table_index, uint32 entry_index, uint32 datareg_index ,uint32 *pData);

/* Function Name:
 *      rtk_diag_tableEntry_write
 * Description:
 *      Write a specific table entry
 * Input:
 *      unit - unit id
 *      table_index - table index number
 *      entry_index - entry's start index for dump
 *      pData       - table entry data
 *      datareg_num - table entry data reg number
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
 *      [SDK_3.6.2]
 *          New added function.
 */
extern int32
rtk_diag_tableEntry_write(uint32 unit, uint32 table_index, uint32 entry_index, uint32 *pData, uint32 datareg_num);


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


/* Function Name:
 *      rtk_diag_reg_get
 * Description:
 *      Get the specified register value in for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
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
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_reg_get(uint32 unit, uint32 reg, uint32 *pValue);

/* Function Name:
 *      rtk_diag_reg_set
 * Description:
 *      Set the specified register value for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
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
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_reg_set(uint32 unit, uint32 reg, uint32 *pValue);

/* Function Name:
 *      rtk_diag_regField_get
 * Description:
 *      Get the specified register-field value for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
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
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_regField_get(uint32 unit, uint32 reg, uint32 field, uint32 *pValue);

/* Function Name:
 *      rtk_diag_regField_set
 * Description:
 *      Set the specified register-field value for debugging
 * Input:
 *      unit    - unit id
 *      reg     - register id
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
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_regField_set(uint32 unit, uint32 reg, uint32 field, uint32 *pValue);



#if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
/* Function Name:
 *      rtk_diag_regInfoByStr_get
 * Description:
 *      Find the REG information by the register name
 * Input:
 *      unit     - unit id
 *      reg_name - register name
 * Output:
 *      pReg_info - REG information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_regInfoByStr_get(uint32 unit, char *reg_name, rtk_diag_regInfo_t *pReg_info);

/* Function Name:
 *      rtk_diag_regFieldInfo_get
 * Description:
 *      Get the REG Filed information
 * Input:
 *      unit  - unit id
 *      reg   - register index
 *      field - field index
 * Output:
 *      pField_info - REG field information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_regFieldInfo_get(uint32 unit, uint32 reg, uint32 field, rtk_diag_regFieldInfo_t *pField_info);


/* Function Name:
 *      rtk_diag_regInfoByStr_match
 * Description:
 *      Seaching the next matched REG name and return REG info
 * Input:
 *      unit  - unit id
 *      reg_name - register name
 *      reg      - start REG index
 * Output:
 *      pReg_info - REG information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      It performs partially match with the REG name.
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_regInfoByStr_match(uint32 unit, char *reg_name, int32 reg, rtk_diag_regInfo_t *pReg_info);
#endif

#if defined (CONFIG_SDK_DUMP_TABLE_WITH_NAME)
/* Function Name:
 *      rtk_diag_tableInfoByStr_get
 * Description:
 *      Find the TBL info by the given name.
 * Input:
 *      unit      - unit id
 *      tbl_name  - TBL name
 * Output:
 *      pTbl_info - TBL info
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The given name must be the same with table name otherwise not found.
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_tableInfoByStr_get(uint32 unit, char *tbl_name, rtk_diag_tblInfo_t *pTbl_info);
/* Function Name:
 *      rtk_diag_tableFieldInfo_get
 * Description:
 *      Find the RTK TBL field info
 * Input:
 *      unit      - unit id
 *      tbl       - TBL index
 *      field     - field index
 * Output:
 *      pField_info - TBL field info
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *       The given name must be the same with table name otherwise not found.
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_tableFieldInfo_get(uint32 unit, uint32 tbl, uint32 field, rtk_diag_tblFieldInfo_t *pField_info);

/* Function Name:
 *      rtk_diag_tableInfoByStr_match
 * Description:
 *      Find the next RTK TBL info by matching keyword
 * Input:
 *      unit      - unit id
 *      keyword   - keyword for matching REG name
 *      tbl       - starting TBL index
 * Output:
 *      pReg_info - TBL info
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      It performs partially match with the TBL name.
 * Changes:
 *      [SDK_3.6.2]
 *          New added function.
 */
int32
rtk_diag_tableInfoByStr_match(uint32 unit, char *keyword, int32 tbl, rtk_diag_tblInfo_t *pTbl_info);
#endif


#endif /* __RTK_DIAG_H__ */

