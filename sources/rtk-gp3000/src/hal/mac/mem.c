/*
 * Copyright (C) 2011 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <osal/memory.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/chipdef/apollo/apollo_table_struct.h>
#include <hal/common/halctrl.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/mac/drv.h>
#include <osal/sem.h>


/*
 * Symbol Definition
 */
#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/spinlock.h>
#if defined(CONFIG_KERNEL_2_6_30)
spinlock_t rtkTableSpinLock = SPIN_LOCK_UNLOCKED; //used when each RG API is called.
#else
DEFINE_SPINLOCK(rtkTableSpinLock);
#endif
unsigned long rtkTableSpinflags; 

#define RTK_TABLE_LOCK      spin_lock_irqsave(&rtkTableSpinLock,rtkTableSpinflags)
#define RTK_TABLE_UNLOCK    spin_unlock_irqrestore(&rtkTableSpinLock,rtkTableSpinflags)
#else
#define RTK_TABLE_LOCK    
#define RTK_TABLE_UNLOCK 
#endif

/*
 * Macro Definition
 */
#define RTK_SDK_WORD_BYTE  4
/*
 * Function Declaration
 */

/* Public Function Body */

/* Function Name:
 *      table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
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
int32
table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    hal_control_t   *pHalCtrl;
    int32 ret;

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_table_read)
        return RT_ERR_FAILED;

    RTK_TABLE_LOCK;
    ret = MACDRV(pHalCtrl)->fMdrv_table_read(table, addr, pData);
    RTK_TABLE_UNLOCK;

    return ret;
} /* end of table_read */


/* Function Name:
 *      table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
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
int32
table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    hal_control_t   *pHalCtrl;
    int32 ret;

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_table_write)
        return RT_ERR_FAILED;

    RTK_TABLE_LOCK;
    ret = (MACDRV(pHalCtrl)->fMdrv_table_write(table, addr, pData));
    RTK_TABLE_UNLOCK;

    return ret;
} /* end of table_write */

/* Function Name:
 *      table_clear
 * Description:
 *      Write one specified table entry by table index.
 * Input:
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
int32
table_clear(
    uint32  table,
    uint32  start,
    uint32  end)
{
    hal_control_t   *pHalCtrl;
    int32 ret;

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    if (NULL == MACDRV(pHalCtrl)->fMdrv_table_clear)
        return RT_ERR_FAILED;

    RTK_TABLE_LOCK;
    ret = (MACDRV(pHalCtrl)->fMdrv_table_clear(table, start, end));
    RTK_TABLE_UNLOCK;

    return ret;
} /* end of table_write */

/* Function Name:
 *      table_field_get
 * Description:
 *      Get the value from one specified field of table in buffer.
 * Input:
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
int32
table_field_get(
    uint32  table,
    uint32  field,
    uint32  *pValue,
    uint32  *pData)
{
    int32               field_bit_pos, data_word_pos, data_bit_pos;
    int32               i, unprocess_len;
    rtk_table_t         *pTable = NULL;
    rtk_tableField_t    *pTblField = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "table=%d, field=%d", table, field);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(((NULL == pValue) || (NULL == pData)), RT_ERR_NULL_POINTER);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip*/
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((field >= pTable->field_num), RT_ERR_OUT_OF_RANGE);

    pTblField = &pTable->fields[field];

    /* Base on pTblField->lsp and pTblField->len to process */
    field_bit_pos = pTblField->lsp;

    /* Caculate the data LSB bit */
    data_word_pos = pTable->datareg_num - 1 - (field_bit_pos >> 5);
    data_bit_pos = field_bit_pos & 31;
    i = 0;

    /* Process single bit request */
    if (1 == pTblField->len)
    {
        if (pData[data_word_pos] & (1 << data_bit_pos))
            pValue[0] = 1;
        else
            pValue[0] = 0;
        return RT_ERR_OK;
    }

    /* Process multiple bits request - can process more than 32-bits in one field  */
    for (unprocess_len = pTblField->len; unprocess_len > 0; unprocess_len -= 32, i++)
    {
        if (data_bit_pos)
        {
            pValue[i] = (pData[data_word_pos] >> data_bit_pos) & ((1 << (32 - data_bit_pos)) - 1);
            if(data_word_pos)
            {
                /* data_word_pos haven't reach the last one */
                data_word_pos--;
                pValue[i] |= (pData[data_word_pos] << (32 - data_bit_pos));
            }
        }
        else
        {
            pValue[i] = pData[data_word_pos];
            data_word_pos--;
        }

        if (unprocess_len < 32)
        {
            pValue[i] &= ((1 << unprocess_len) - 1);
        }
    }

    return RT_ERR_OK;
} /* end of table_field_get */


/* Function Name:
 *      table_field_set
 * Description:
 *      Set the value to one specified field of table in buffer.
 * Input:
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
 *      1. The API is used when *pValue argument is WORD type.
 */
int32
table_field_set(
    uint32  table,
    uint32  field,
    uint32  *pValue,
    uint32  *pData)
{
    uint32              masks;
    int32               field_bit_pos, data_word_pos, data_bit_pos;
    int32               i, unprocess_len;
    rtk_table_t         *pTable = NULL;
    rtk_tableField_t    *pTblField = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "table=%d, field=%d", table, field);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip  */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((field >= pTable->field_num), RT_ERR_OUT_OF_RANGE);

    pTblField = &pTable->fields[field];

    /* Base on pTblField->lsp and pTblField->len to process */
    field_bit_pos = pTblField->lsp;

    /* Caculate the data LSB bit */
    data_word_pos = pTable->datareg_num - 1 - (field_bit_pos >> 5);
    data_bit_pos = field_bit_pos & 31;
    i = 0;

    /* Process multiple bits request - can process more than 32-bits in one field  */
    for (unprocess_len = pTblField->len; unprocess_len > 0; unprocess_len -= 32, i++)
    {
        if (data_bit_pos)
        {
            if (unprocess_len >= 32)
            {
                masks = -1;
            }
            else
            {
                masks = (1 << unprocess_len) - 1;
                pValue[i] &= masks;
            }

            pData[data_word_pos] &= ~(masks << data_bit_pos);
            pData[data_word_pos] |= (pValue[i] << data_bit_pos);
            if(data_word_pos)
            {
                /* data_word_pos haven't reach the last one */
                data_word_pos--;
                pData[data_word_pos] &= ~(masks >> (32 - data_bit_pos));
                pData[data_word_pos] |= (pValue[i] >> (32 - data_bit_pos)) & ((1 << data_bit_pos) - 1);
            }
        }
        else
        {
            if (unprocess_len >= 32)
            {
                pData[data_word_pos] = pValue[i];
            }
            else
            {
                masks = (1 << unprocess_len) - 1;
                pValue[i] &= masks;
                pData[data_word_pos] &= ~masks;
                pData[data_word_pos] |= pValue[i];
            }
            data_word_pos--;
        }
    }

    return RT_ERR_OK;
} /* end of table_field_set */




/* Function Name:
 *      table_field_byte_get
 * Description:
 *      Get the value from one specified field of table in buffer.
 * Input:
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
 *      1. The API is used when *pValue argument is BYTE type,
 *         like mac address ... etc.
 */
int32
table_field_byte_get(
    uint32  table,
    uint32  field,
    uint8   *pValue,
    uint32  *pData)
{
    uint32              *pBytes_buffer = NULL;
    uint32              field_words = 0, field_bytes = 0;
    int32               i, j, k, ret = RT_ERR_FAILED;
    int32               field_bit_pos;
    rtk_table_t         *pTable = NULL;
    rtk_tableField_t    *pTblField = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "table=%d, field=%d", table, field);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((field >= pTable->field_num), RT_ERR_OUT_OF_RANGE);

    pTblField = &pTable->fields[field];

    /* Base on pTblField->lsp and pTblField->len to process */
    field_bit_pos = pTblField->lsp;

    /* Caculate the field need use how many bytes or words */
    field_words = ((pTblField->len + 31)/32);
    field_bytes = ((pTblField->len + 7)/8);

    /* Allocate word based buffer for store table_field_get return word array */
    if ((pBytes_buffer = (uint32 *) osal_alloc (sizeof(uint32) * field_words)) == NULL)
        return RT_ERR_FAILED;

    /* Do word array based field get */
    if ((ret = table_field_get(table, field, pBytes_buffer, pData)) != RT_ERR_OK)
    {
        osal_free (pBytes_buffer);
        return ret;
    }

    /* Translate word array into byte array */
    for (i = 0, j = field_bytes - 1; i < field_words; i++)
    {
        for (k = 0; k < RTK_SDK_WORD_BYTE && j >= 0; j--, k++)
            (*(pValue + j)) = ((*(pBytes_buffer + i)) >> ((k % RTK_SDK_WORD_BYTE) << 3)) & 0xFF;
    }

    osal_free (pBytes_buffer);
    return RT_ERR_OK;
} /* end of table_field_byte_get */


/* Function Name:
 *      table_field_byte_set
 * Description:
 *      Set the value to one specified field of table in buffer.
 * Input:
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
 *      1. The API is used when *pValue argument is BYTE type,
 *         like mac address ... etc.
 */
int32
table_field_byte_set(
    uint32  table,
    uint32  field,
    uint8   *pValue,
    uint32  *pData)
{
    uint32              *pBytes_buffer = NULL;
    uint32              field_words = 0, field_bytes = 0;
    int32               i, j, k, ret = RT_ERR_FAILED;
    int32               field_bit_pos;
    rtk_table_t         *pTable = NULL;
    rtk_tableField_t    *pTblField = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "table=%d, field=%d", table, field);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip*/
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((field >= pTable->field_num), RT_ERR_OUT_OF_RANGE);

    pTblField = &pTable->fields[field];

    /* Base on pTblField->lsp and pTblField->len to process */
    field_bit_pos = pTblField->lsp;

    /* Caculate the field need use how many bytes or words */
    field_words = ((pTblField->len + 31)/32);
    field_bytes = ((pTblField->len + 7)/8);

    /* Allocate word based buffer for store table_field_get return word array */
    if ((pBytes_buffer = (uint32 *) osal_alloc (sizeof(uint32) * field_words)) == NULL)
        return RT_ERR_FAILED;

    /* Translate byte array into word array for input to table_field_set */
    for (i = 0, j = field_bytes - 1; i < field_words; i++)
    {
        (*(pBytes_buffer + i)) = 0;
        for (k = 0; k < RTK_SDK_WORD_BYTE && j >= 0; j--, k++)
        {
            (*(pBytes_buffer + i)) |= ((uint32) *(pValue+j) << ((k % 4) << 3));
        }
    }

    /* Do word array based field set */
    if ((ret = table_field_set(table, field, pBytes_buffer, pData)) != RT_ERR_OK)
    {
        osal_free (pBytes_buffer);
        return ret;
    }

    osal_free (pBytes_buffer);
    return RT_ERR_OK;
} /* end of table_field_byte_set */


/* Function Name:
 *      table_field_mac_get
 * Description:
 *      Get the mac address from one specified field of table in buffer.
 * Input:
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
int32
table_field_mac_get(
    uint32  table,
    uint32  field,
    uint8   *pValue,
    uint32  *pData)
{
    uint32  ret = RT_ERR_FAILED;
    uint32  temp_mac[2];

    if ((ret = table_field_get(table, field, &temp_mac[0], pData)) != RT_ERR_OK)
        return ret;

    *(pValue + 0) = (uint8) ((temp_mac[1] & 0x0000FF00) >> 8);
    *(pValue + 1) = (uint8) (temp_mac[1] & 0x000000FF);
    *(pValue + 2) = (uint8) ((temp_mac[0] & 0xFF000000) >> 24);
    *(pValue + 3) = (uint8) ((temp_mac[0] & 0x00FF0000) >> 16);
    *(pValue + 4) = (uint8) ((temp_mac[0] & 0x0000FF00) >> 8);
    *(pValue + 5) = (uint8) (temp_mac[0] & 0x000000FF);

    return RT_ERR_OK;
} /* end of table_field_mac_get */


/* Function Name:
 *      table_field_mac_set
 * Description:
 *      Set the mac address to one specified field of table in buffer.
 * Input:
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
int32
table_field_mac_set(
    uint32  table,
    uint32  field,
    uint8   *pValue,
    uint32  *pData)
{
    uint32  ret = RT_ERR_FAILED;
    uint32  temp_mac[2];

    temp_mac[0] = (((uint32) *(pValue + 2)) << 24) | (((uint32) *(pValue + 3)) << 16) |
                   (((uint32) *(pValue + 4)) << 8) | ((uint32) *(pValue + 5));
    temp_mac[1] = (((uint32) *(pValue + 0)) << 8) | ((uint32) *(pValue + 1));

    if ((ret = table_field_set(table, field, temp_mac, pData)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of table_field_mac_set */

/* Function Name:
 *      table_size_get
 * Description:
 *      Get size of specified table
 * Input:
 *      table - table index
 * Output:
 *      pSize - pointer buffer of size of table
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_NULL_POINTER       - input parameter is null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 */
int32
table_size_get(
    uint32  table,
    uint32  *pSize)
{
    rtk_table_t *pTable = NULL;
    
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pSize), RT_ERR_NULL_POINTER);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);

    *pSize = pTable->size;
    
    return RT_ERR_OK;
} /* end of table_size_get */

