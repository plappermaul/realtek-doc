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
 * Purpose : table service APIs in the SDK.
 *
 * Feature : table service APIs
 *
 */

#ifndef __HAL_MAC_MEM_H__
#define __HAL_MAC_MEM_H__

/*
 * Include Files
 */
#include <hal/chipdef/allmem.h>

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      unit  - unit id
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
extern int32
table_read(
    uint32  unit,
    uint32  table,
    uint32  addr,
    uint32  *pData);

/* Function Name:
 *      table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
 *      unit  - unit id
 *      table - table index
 *      addr  - entry address of the table
 *      pData - pointer buffer of table entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
extern int32
table_write(
    uint32  unit,
    uint32  table,
    uint32  addr,
    uint32  *pData);

/* Function Name:
 *      table_field_get
 * Description:
 *      Get the value from one specified field of table in buffer.
 * Input:
 *      unit   - unit id
 *      table  - table index
 *      field  - field index of the table
 *      pData  - pointer buffer of table entry data
 * Output:
 *      pValue - pointer buffer of value from the specified field of table
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_NULL_POINTER       - input parameter is null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 *      1. The API is used when *value argument is WORD type.
 */
extern int32
table_field_get(
    uint32  unit,
    uint32  table,
    uint32  field,
    uint32  *pValue,
    uint32  *pData);

/* Function Name:
 *      table_field_set
 * Description:
 *      Set the value to one specified field of table in buffer.
 * Input:
 *      unit   - unit id
 *      table  - table index
 *      field  - field index of the table
 *      pValue - pointer buffer of value from the specified field of table
 * Output:
 *      pData  - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_NULL_POINTER       - input parameter is null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 *      1. The API is used when *value argument is WORD type.
 */
extern int32
table_field_set(
    uint32  unit,
    uint32  table,
    uint32  field,
    uint32  *pValue,
    uint32  *pData);

/* Function Name:
 *      table_field_byte_get
 * Description:
 *      Get the value from one specified field of table in buffer.
 * Input:
 *      unit   - unit id
 *      table  - table index
 *      field  - field index of the table
 *      pData  - pointer buffer of table entry data
 * Output:
 *      pValue - pointer buffer of value from the specified field of table
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_NULL_POINTER       - input parameter is null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 *      1. The API is used when *value argument is BYTE type,
 *         like mac address ... etc.
 */
extern int32
table_field_byte_get(
    uint32  unit,
    uint32  table,
    uint32  field,
    uint8   *pValue,
    uint32  *pData);

/* Function Name:
 *      table_field_byte_set
 * Description:
 *      Set the value to one specified field of table in buffer.
 * Input:
 *      unit   - unit id
 *      table  - table index
 *      field  - field index of the table
 *      pValue - pointer buffer of value from the specified field of table
 * Output:
 *      pData  - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_NULL_POINTER       - input parameter is null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 *      1. The API is used when *value argument is BYTE type,
 *         like mac address ... etc.
 */
extern int32
table_field_byte_set(
    uint32  unit,
    uint32  table,
    uint32  field,
    uint8   *pValue,
    uint32  *pData);

/* Function Name:
 *      table_field_mac_get
 * Description:
 *      Get the mac address from one specified field of table in buffer.
 * Input:
 *      unit   - unit id
 *      table  - table index
 *      field  - field index of the table
 *      pData  - pointer buffer of table entry data
 * Output:
 *      pValue - pointer buffer of value from the specified field of table
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. The API is used for mac address type only
 */
extern int32
table_field_mac_get(
    uint32  unit,
    uint32  table,
    uint32  field,
    uint8   *pValue,
    uint32  *pData);

/* Function Name:
 *      table_field_mac_set
 * Description:
 *      Set the mac address to one specified field of table in buffer.
 * Input:
 *      unit   - unit id
 *      table  - table index
 *      field  - field index of the table
 *      pValue - pointer buffer of value from the specified field of table
 * Output:
 *      pData  - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      1. The API is used for mac address type only
 */
extern int32
table_field_mac_set(
    uint32  unit,
    uint32  table,
    uint32  field,
    uint8   *pValue,
    uint32  *pData);

/* Function Name:
 *      table_size_get
 * Description:
 *      Get size of specified table
 * Input:
 *      unit  - unit id
 *      table - table index
 *      pSize - pointer buffer of size of table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_NULL_POINTER       - input parameter is null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 */
extern int32
table_size_get(
    uint32  unit,
    uint32  table,
    uint32  *pSize);

/* Function Name:
 *      table_find1
 * Description:
 *      Find this kind of table structure in this specified chip.
 * Input:
 *      unit  - unit id
 *      table - table index
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of table structure that found
 * Note:
 *      None
 */
extern rtk_table_t *
table_find (uint32 unit, uint32 table);


#endif  /* __HAL_MAC_MEM_H__ */
