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
 * Purpose : register service APIs in the SDK.
 *
 * Feature : register service APIs
 *
 */

/*
 * Include Files
 */

#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/module.h>
#endif

#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <ioal/mem32.h>
#include <hal/chipdef/allreg.h>
//#include <hal/chipdef/apollo/apollo_reg_struct.h>
//#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/common/halctrl.h>
#include <hal/mac/reg.h>



/*
 * Symbol Definition
 */
#define REG_WORD_MAX    50  /* assume maximum reg_word is 10 */

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
static rtk_regField_t *_reg_field_find(uint32 reg, uint32 field);
static int32 _reg_addr_find(uint32 reg, int32 index_1, int32 index_2, uint32 *pAddr);

#if 0  /*for register performance issue*/
static int32
_reg_array_field_get(
    uint32  reg,
    int32   index_1,
    int32   index_2,
    uint32  field,
    uint32  *pValue,
    uint32  *pData);
static int32
_reg_array_field_set(
    uint32  reg,
    int32   index_1,
    int32   index_2,
    uint32  field,
    uint32  *pValue,
    uint32  *pData);
#endif

/* Static Function Body */

/* Function Name:
 *      _reg_field_find
 * Description:
 *      Find this kind of register field structure in this specified chip.
 * Input:
 *      reg   - register index
 *      field - field index
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of register field structure that found
 * Note:
 *      None
 */
static rtk_regField_t *
_reg_field_find(uint32 reg, uint32 field)
{
    uint32          i;
    rtk_regField_t  *pField = NULL;

    pField = HAL_GET_REG_FIELDS(reg);
    if(pField == NULL)
    {
        return NULL;
    }
    for (i = 0; i < HAL_GET_REG_FIELD_NUM(reg); i++, pField++)
    {
        if (pField->name == field)
           return pField;
    }
    return NULL;    /* field not found */
} /* end of _reg_field_find */


/* Public Function Body */

/* Function Name:
 *      reg_read
 * Description:
 *      Read one specified register from the chip or lower layer I/O.
 * Input:
 *      reg    - register index
 * Output:
 *      pValue - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
int32
reg_read(uint32 reg, uint32 *pValue)
{
    int32   ret = RT_ERR_FAILED;
    uint32  addr = 0, num = 0, i;

    RT_LOG(LOG_TRACE, MOD_HAL, "reg=%d", reg);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    num = HAL_GET_REG_WORD_NUM(reg);
    for (i = 0; i < num; i++)
    {
        ret = ioal_mem32_read(addr+4*i, pValue+i);
    }

    return ret;
} /* end of reg_read */


/* Function Name:
 *      reg_write
 * Description:
 *      Write one specified register to the chip or lower layer I/O.
 * Input:
 *      reg   - register index
 *      pValue - pointer buffer of register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. Use semaphore for prevent race condition with reg_field_write
 *      2. The function is supported 32-bit register access.
 */
int32
reg_write(uint32 reg, uint32 *pValue)
{
    int32           ret = RT_ERR_FAILED;
    uint32  addr = 0, num = 0, i;

    RT_LOG(LOG_TRACE, MOD_HAL, "reg=%d, value=0x%x", reg, *pValue);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    if ((ret = _reg_addr_find(reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    num = HAL_GET_REG_WORD_NUM(reg);
    for (i = 0; i < num; i++)
    {
        {
            ret = ioal_mem32_write(addr+4*i, *(pValue+i));
        }
    }

    return ret;
} /* end of reg_write */


/* Function Name:
 *      reg_field_get
 * Description:
 *      Get the value from one specified field of register in buffer.
 * Input:
 *      reg    - register index
 *      field  - field index of the register
 *      pData  - pointer buffer of register data
 * Output:
 *      pValue - pointer buffer of value from the specified field of register
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
int32
reg_field_get(
    uint32  reg,
    uint32  field,
    uint32  *pValue,
    uint32  *pData)
{

    uint32  field_bit_pos, reg_word_num;
    uint32  data_word_pos, data_bit_pos;
    int32  i, unprocess_len;
    rtk_regField_t  *pField = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "reg=%d, field=%d, data=0x%x ", reg, field, pData[0]);
    RT_PARAM_CHK(((reg >= HAL_GET_MAX_REG_IDX()) || (field >= HAL_GET_MAX_REGFIELD_IDX())), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    pField = _reg_field_find(reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);


    /* Base on pField->lsp and pField->len to process */
    field_bit_pos = pField->lsp;

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 5);
    data_bit_pos = field_bit_pos & 31;
    i = 0;


    /* Process single bit request */
    if (1 == pField->len)
    {
        if (pData[data_word_pos] & (1 << data_bit_pos))
            pValue[0] = 1;
        else
            pValue[0] = 0;
        return RT_ERR_OK;
    }

    /* Process multiple bits request - can process more than 32-bits in one field  */
    for (unprocess_len = pField->len; unprocess_len > 0; unprocess_len -= 32, i++)
    {
        if (data_bit_pos)
        {
            pValue[i] = (pData[data_word_pos] >> data_bit_pos) & ((1 << (32 - data_bit_pos)) - 1);
            data_word_pos--;
            pValue[i] |= (pData[data_word_pos] << (32 - data_bit_pos));
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
} /* end of reg_field_get */


/* Function Name:
 *      reg_field_set
 * Description:
 *      Set the value to one specified field of register in buffer.
 * Input:
 *      reg    - register index
 *      field  - field index of the register
 *      pValue - pointer buffer of value from the specified field of register
 * Output:
 *      pData  - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
int32
reg_field_set(
    uint32  reg,
    uint32  field,
    uint32  *pValue,
    uint32  *pData)
{
    uint32  masks;
    uint32  field_bit_pos, reg_word_num;
    uint32  data_word_pos, data_bit_pos;
    int32  i, unprocess_len;
    rtk_regField_t  *pField = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "reg=%d, field=%d, value=0x%x", reg, field, *pValue);
    RT_PARAM_CHK(((reg >= HAL_GET_MAX_REG_IDX()) || (field >= HAL_GET_MAX_REGFIELD_IDX())), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pField = _reg_field_find(reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);


    /* Base on pField->lsp and pField->len to process */
    field_bit_pos = pField->lsp;

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 5);
    data_bit_pos = field_bit_pos & 31;
    i = 0;

    /* Process multiple bits request - can process more than 32-bits in one field  */
    for (unprocess_len = pField->len; unprocess_len > 0; unprocess_len -= 32, i++)
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
            if (data_word_pos) {
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
            if (data_word_pos) {
                data_word_pos--;
            }
        }
    }
    
    return RT_ERR_OK;
} /* end of reg_field_set */


/* Function Name:
 *      reg_field_read
 * Description:
 *      Read the value from one specified field of register in chip.
 * Input:
 *      reg    - register index
 *      field  - field index of the register
 * Output:
 *      pValue - pointer buffer of value from the specified field of register
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
int32
reg_field_read(
    uint32  reg,
    uint32  field,
    uint32  *pValue)
{
#if 1 /* Performance enhance: access single register of whole software register table */
    uint32  reg_word_num, data_word_pos;
    uint32  data, mask;
    uint32  addr = 0, mod32_lsp;
    int32   ret = RT_ERR_FAILED;
    rtk_regField_t  *pField = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "reg=%d, field=%d",reg, field);
    
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    reg_word_num = HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (pField->lsp >> 5);
    addr += data_word_pos*4;
    mod32_lsp = pField->lsp & 31;

    if (ioal_mem32_read(addr, &data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    
    if (pField->len < 32)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFFFFFF;
 
    (*pValue) = (data >> mod32_lsp) & mask;
    return RT_ERR_OK;
#else /* Old code, access whole software register table */
    uint32  data[REG_WORD_MAX];
    uint32  addr = 0, num = 0, i;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "reg=%d, field=%d", reg, field);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    num = HAL_GET_REG_WORD_NUM(reg);
    if (num > REG_WORD_MAX)
        return RT_ERR_FAILED;
    
    for (i = 0; i < num; i++)
    {
        if (ioal_mem32_read(addr+4*i, (uint32 *)(data+i)) != RT_ERR_OK)
        return RT_ERR_FAILED;
    }

    if (reg_field_get(reg, field, pValue, (uint32 *)(data)) != RT_ERR_OK)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
#endif
} /* end of reg_field_read */


/* Function Name:
 *      reg_field_write
 * Description:
 *      Write the value to one specified field of register in chip.
 * Input:
 *      reg   - register index
 *      field - field index of the register
 *      pValue - value from the specified field of register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. Use semaphore for prevent race condition with reg_write
 *      2. The function is supported 32-bit register access.
 */
int32
reg_field_write(
    uint32  reg,
    uint32  field,
    uint32  *pValue)
{
#if 1 /* Performance enhance: access single register of whole software register table */
    uint32 reg_word_num, data_word_pos;
    uint32 data, mask;
    uint32 addr = 0;
    uint32 mod32_lsp;
    int32 ret = RT_ERR_FAILED;
    rtk_regField_t *pField = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "reg=%d, field=%d, value=0x%x", reg, field, *pValue);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    reg_word_num = HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (pField->lsp >> 5);
    addr += data_word_pos*4;
    mod32_lsp = pField->lsp & 31;

    if (ioal_mem32_read(addr, &data) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    if (pField->len < 32)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFFFFFF;

    data &= ~(mask << mod32_lsp);
    data |= (((*pValue) & mask) << mod32_lsp);

    if (ioal_mem32_write(addr, data) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
#else
    uint32  data[REG_WORD_MAX], r_data[REG_WORD_MAX];
    uint32  addr = 0, num = 0, i;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "reg=%d, field=%d, value=0x%x", reg, field, *pValue);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    if ((ret = _reg_addr_find(reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

     num = HAL_GET_REG_WORD_NUM(reg);
    if (num > REG_WORD_MAX)
    {
        return RT_ERR_FAILED;
    }

    for (i = 0; i < num; i++)
    {
        if (ioal_mem32_read(addr+4*i, (uint32 *)(r_data+i)) != RT_ERR_OK)
        {
            return RT_ERR_FAILED;
        }
    }

    //data = r_data;
    osal_memcpy(data, r_data, sizeof(data));
    if (reg_field_set(reg, field, pValue, (uint32 *)(data)) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    /*if (data != r_data)*/
#if 0 /* no compare, just write data. */
    if (osal_memcmp(data, r_data, sizeof(data)) != 0)
#endif
    {
        for (i = 0; i < num; i++)
        {
            if (ioal_mem32_write(addr+4*i, data[i]) != RT_ERR_OK)
            {
                return RT_ERR_FAILED;
            }
        }
    }


    return RT_ERR_OK;
#endif
} /* end of reg_field_write */


/* Function Name:
 *      reg_idx2Addr_get
 * Description:
 *      Register index to physical address transfer in chip.
 * Input:
 *      reg   - register index
 * Output:
 *      pAddr - pointer buffer of register address
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32 
reg_idx2Addr_get(uint32 reg, uint32 *pAddr)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "reg=%d",  reg);
    RT_PARAM_CHK((pAddr == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);

    if ((ret = _reg_addr_find(reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, pAddr)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;   
} /* end of reg_idx2Addr_get */


/* Function Name:
 *      reg_idxMax_get
 * Description:
 *      Get the maximum value of register index in chip.
 * Input:
 *      reg  - register index
 * Output:
 *      pMax - pointer buffer of maximum value of register index
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
int32 
reg_idxMax_get(uint32 *pMax)
{
    RT_PARAM_CHK((pMax == NULL), RT_ERR_NULL_POINTER);

    (*pMax) = HAL_GET_MAX_REG_IDX();
    
    return RT_ERR_OK;   
} /* end of reg_idxMax_get */


/*#################################################*/
/* Following APIS are for register array mechanism */
/*#################################################*/

/* Function Name:
 *      _reg_addr_find
 * Description:
 *      Calculate one specified register array address.
 * Input:
 *      reg     - register index
 *      index_1 - dimension 1 index
 *      index_2 - dimension 2 index
 * Output:
 *      pAddr   - pointer buffer of register address
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_REG_ARRAY_INDEX_1 - invalid index 1 of register array
 *      RT_ERR_REG_ARRAY_INDEX_2 - invalid index 2 of register array
 * Note:
 *      The function is supported 32-bit register access.
 */
static int32
_reg_addr_find(uint32 reg, int32 index_1, int32 index_2, uint32 *pAddr)
{
    uint32 dim1_size = 0;
    int32 ret = RT_ERR_FAILED;
    uint32 isPpReg = 0;
    hal_control_t *pHalCtrl = NULL;

    *pAddr = HAL_GET_REG_ADDR(reg);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    /* Check index_1 & index_2 valid or not? */
    if ((index_1 != REG_ARRAY_INDEX_NONE) && ((index_1 < HAL_GET_REG_LPORT(reg)) || (index_1 > HAL_GET_REG_HPORT(reg))))
        return RT_ERR_REG_ARRAY_INDEX_1;
    
    if ((index_2 != REG_ARRAY_INDEX_NONE) && ((index_2 < HAL_GET_REG_LARRAY(reg)) || (index_2 > HAL_GET_REG_HARRAY(reg))))
        return RT_ERR_REG_ARRAY_INDEX_2;

    if ((index_1 != REG_ARRAY_INDEX_NONE) && (0 == HAL_GET_REG_HPORT(reg)))
        return RT_ERR_REG_ARRAY_INDEX_1;

    if ((index_2 != REG_ARRAY_INDEX_NONE) && (0 == HAL_GET_REG_HARRAY(reg)))
        return RT_ERR_REG_ARRAY_INDEX_2;


    if (hal_isPpBlock_check((*pAddr), &isPpReg) != RT_ERR_OK)
        return ret;

    if (isPpReg)
    {
        /* PER_PORT */
        if (index_1 != REG_ARRAY_INDEX_NONE)
            *pAddr += (index_1 * HAL_GET_MACPP_INTERVAL());
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            if (HAL_GET_REG_ARRAY_OFFSET(reg) % 32)
                *pAddr += ((index_2 - HAL_GET_REG_LARRAY(reg))/(32/HAL_GET_REG_ARRAY_OFFSET(reg)))*4;
            else
                *pAddr += (index_2 - HAL_GET_REG_LARRAY(reg)) * (HAL_GET_REG_ARRAY_OFFSET(reg)/8);
        }
    }
    else
    {
        /* Global */
        if ((index_1 != REG_ARRAY_INDEX_NONE) && (index_2 != REG_ARRAY_INDEX_NONE))
        {
            /* calculate index_1 dimension size */
            if (HAL_GET_REG_ARRAY_OFFSET(reg) % 32)
                dim1_size = (((HAL_GET_REG_HARRAY(reg) - HAL_GET_REG_LARRAY(reg))/(32/HAL_GET_REG_ARRAY_OFFSET(reg)))+1)*4;
            else
                dim1_size = ((HAL_GET_REG_HARRAY(reg)-HAL_GET_REG_LARRAY(reg))+1)*(HAL_GET_REG_ARRAY_OFFSET(reg)/8);
            
            /* Add index 1 offset */
            *pAddr += (index_1 - HAL_GET_REG_LPORT(reg)) * dim1_size;
            
            /* Add index 2 offset */
            if (HAL_GET_REG_ARRAY_OFFSET(reg) % 32)
                *pAddr += ((index_2-HAL_GET_REG_LARRAY(reg))/(32/HAL_GET_REG_ARRAY_OFFSET(reg)))*4;
            else
                *pAddr += (index_2 - HAL_GET_REG_LARRAY(reg)) * (HAL_GET_REG_ARRAY_OFFSET(reg)/8);
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            if (HAL_GET_REG_ARRAY_OFFSET(reg) % 32)
                *pAddr += ((index_1 - HAL_GET_REG_LPORT(reg))/(32/HAL_GET_REG_ARRAY_OFFSET(reg)))*4;
            else
                *pAddr += (index_1 - HAL_GET_REG_LPORT(reg)) * (HAL_GET_REG_ARRAY_OFFSET(reg)/8);
        }
        else if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            if (HAL_GET_REG_ARRAY_OFFSET(reg) % 32)
                *pAddr += ((index_2 - HAL_GET_REG_LPORT(reg))/(32/HAL_GET_REG_ARRAY_OFFSET(reg)))*4;
            else
                *pAddr += (index_2 - HAL_GET_REG_LPORT(reg)) * (HAL_GET_REG_ARRAY_OFFSET(reg)/8);
        }
    }

    
    return RT_ERR_OK;   
} /* end of _reg_addr_find */

/* Function Name:
 *      reg_array_read
 * Description:
 *      Read one specified register or register array from the chip or lower layer I/O.
 * Input:
 *      reg     - register index
 *      index_1 - dimension 1 index
 *      index_2 - dimension 2 index
 * Output:
 *      pValue  - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
int32
reg_array_read(uint32 reg, int32 index_1, int32 index_2, uint32 *pValue)
{
    uint32  addr = 0, num = 0, i;
    int32   ret = RT_ERR_FAILED;

    RT_DBG(LOG_DEBUG, (MOD_HAL), "reg=%d", reg);

    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    num = HAL_GET_REG_WORD_NUM(reg);

    for (i = 0; i < num; i++)
    {
        if (ioal_mem32_read(addr+4*i, (uint32 *)(pValue+i)) != RT_ERR_OK)
        {
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
} /* end of reg_array_read */

/* Function Name:
 *      reg_array_write
 * Description:
 *      Write one specified register or register array from the chip or lower layer I/O.
 * Input:
 *      reg     - register index
 *      index_1 - dimension 1 index
 *      index_2 - dimension 2 index
 * Output:
 *      pValue  - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
int32
reg_array_write(uint32 reg, int32 index_1, int32 index_2, uint32 *pValue)
{
    uint32  addr = 0, num, i;
    int32   ret = RT_ERR_FAILED;

    RT_DBG(LOG_DEBUG, (MOD_HAL), "reg=%d", reg);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    num = HAL_GET_REG_WORD_NUM(reg);
    for (i = 0; i < num; i++)
    {
        if (ioal_mem32_write(addr+4*i, *(pValue+i)) != RT_ERR_OK)
        {
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
} /* end of reg_array_write */

/* Function Name:
 *      reg_array_field_read
 * Description:
 *      Read the value from one specified field of register or register array in chip.
 * Input:
 *      reg     - register index
 *      index_1 - dimension 1 index
 *      index_2 - dimension 2 index
 *      field   - field index of the register
 * Output:
 *      pValue  - pointer buffer of value from the specified field of register
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
int32
reg_array_field_read(
    uint32  reg,
    int32   index_1,
    int32   index_2,
    uint32  field,
    uint32  *pValue)
{
#if 1 /* Performance enhance: access single register of whole software register table */
    uint32  field_bit_pos = 0, reg_word_num;
    uint32  data_word_pos;
    uint32  data, mask;
    uint32  addr = 0, mod32_lsp;
    int32   ret = RT_ERR_FAILED;
    rtk_regField_t  *pField = NULL;


    RT_DBG(LOG_DEBUG, (MOD_HAL), "reg=%d, field=%d", reg, field);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    if ((HAL_GET_REG_ARRAY_OFFSET(reg) % 32) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-HAL_GET_REG_LARRAY(reg))%(32/HAL_GET_REG_ARRAY_OFFSET(reg))) * HAL_GET_REG_ARRAY_OFFSET(reg));
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-HAL_GET_REG_LPORT(reg))%(32/HAL_GET_REG_ARRAY_OFFSET(reg))) * HAL_GET_REG_ARRAY_OFFSET(reg));
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 5);
    addr += data_word_pos*4;
    mod32_lsp = field_bit_pos & 31;

    if (ioal_mem32_read(addr, &data) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if (pField->len < 32)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFFFFFF;

    (*pValue) = (data >> mod32_lsp) & mask;
    return RT_ERR_OK;
#else
    uint32  data[REG_WORD_MAX], addr, num, i;
    int32   ret = RT_ERR_FAILED;

    RT_DBG(LOG_DEBUG, (MOD_HAL), "reg=%d, field=%d", reg, field);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    num = HAL_GET_REG_WORD_NUM(reg);


    if (num > REG_WORD_MAX)
        return RT_ERR_FAILED;    
    for (i = 0; i < num; i++)
    {
        if (ioal_mem32_read(addr+4*i, (uint32 *)(data+i)) != RT_ERR_OK)
        return RT_ERR_FAILED;
    }
    return _reg_array_field_get(reg, index_1, index_2, field, pValue, (uint32 *)(data));

#endif
} /* end of reg_array_field_read */

/* Function Name:
 *      reg_array_field_write
 * Description:
 *      Write the value to one specified field of register or register array in chip.
 * Input:
 *      reg     - register index
 *      index_1 - dimension 1 index
 *      index_2 - dimension 2 index
 *      field   - field index of the register
 *      pValue  - pointer buffer of value from the specified field of register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. Use semaphore for prevent race condition with reg_write
 *      2. The function is supported 32-bit register access.
 */
int32
reg_array_field_write(
    uint32  reg,
    int32   index_1,
    int32   index_2,
    uint32  field,
    uint32  *pValue)
{
#if 1 /* Performance enhance: access single register of whole software register table */
    uint32  field_bit_pos = 0, reg_word_num;
    uint32  data_word_pos;
    uint32  data, mask;
    uint32  addr = 0, mod32_lsp;
    int32   ret = RT_ERR_FAILED;
    rtk_regField_t  *pField = NULL;

    RT_DBG(LOG_DEBUG, (MOD_HAL), "reg=%d, field=%d", reg, field);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    if ((HAL_GET_REG_ARRAY_OFFSET(reg) % 32) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-HAL_GET_REG_LARRAY(reg))%(32/HAL_GET_REG_ARRAY_OFFSET(reg))) * HAL_GET_REG_ARRAY_OFFSET(reg));
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-HAL_GET_REG_LPORT(reg))%(32/HAL_GET_REG_ARRAY_OFFSET(reg))) * HAL_GET_REG_ARRAY_OFFSET(reg));
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 5);
    addr += data_word_pos*4;
    mod32_lsp = field_bit_pos & 31;

    if (ioal_mem32_read(addr, &data) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    if (pField->len < 32)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFFFFFF;

    data &= ~(mask << mod32_lsp);
    data |= (((*pValue) & mask) << mod32_lsp);

    if (ioal_mem32_write(addr, data) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
#else
    uint32  data[REG_WORD_MAX], addr, num, i;
    int32   ret = RT_ERR_FAILED;

    RT_DBG(LOG_DEBUG, (MOD_HAL), "reg=%d, field=%d, value=0x%x", reg, field, *pValue);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(reg, index_1, index_2, &addr)) != RT_ERR_OK)
    {
        return ret;
    }

    num = HAL_GET_REG_WORD_NUM(reg);
    for (i = 0; i < num; i++)
    {
        if (ioal_mem32_read(addr+4*i, (uint32 *)(data+i)) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }
    }

    if (_reg_array_field_set(reg, index_1, index_2, field, pValue, (uint32 *)(data)) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    for (i = 0; i < num; i++)
    {
        if (ioal_mem32_write(addr+4*i, data[i]) != RT_ERR_OK)
        {
            return RT_ERR_FAILED;
        }
    }
    return RT_ERR_OK;
#endif
} /* end of reg_array_field_write */

#if 0 /*for register performance issue*/
/* Function Name:
 *      _reg_array_field_get
 * Description:
 *      Get the value from one specified field of register or register array in buffer.
 * Input:
 *      reg     - register index
 *      index_1 - dimension 1 index
 *      index_2 - dimension 2 index
 *      field   - field index of the register
 *      pData   - pointer buffer of register data
 * Output:
 *      pValue  - pointer buffer of value from the specified field of register
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
int32
_reg_array_field_get(
    uint32  reg,
    int32   index_1,
    int32   index_2,
    uint32  field,
    uint32  *pValue,
    uint32  *pData)
{
    uint32  field_bit_pos = 0, reg_word_num;
    uint32  data_word_pos, data_bit_pos;
    int32  i, unprocess_len;
    rtk_regField_t  *pField = NULL;

    RT_DBG(LOG_DEBUG, (MOD_HAL), "reg=%d, index_1=%d, index_2=%d, field=%d, data=0x%x", reg, index_1, index_2, field, *pData);
    RT_PARAM_CHK(((reg >= HAL_GET_MAX_REG_IDX()) || (field >= HAL_GET_MAX_REGFIELD_IDX())), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pField = _reg_field_find(reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);


    /* Base on pField->lsp and pField->len to process */
    if ((HAL_GET_REG_ARRAY_OFFSET(reg) % 32) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-HAL_GET_REG_LARRAY(reg))%(32/HAL_GET_REG_ARRAY_OFFSET(reg))) * HAL_GET_REG_ARRAY_OFFSET(reg));
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-HAL_GET_REG_LPORT(reg))%(32/HAL_GET_REG_ARRAY_OFFSET(reg))) * HAL_GET_REG_ARRAY_OFFSET(reg));
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 5);
    data_bit_pos = field_bit_pos & 31;
    i = 0;

    /* Process single bit request */
    if (1 == pField->len)
    {
        if (pData[data_word_pos] & (1 << data_bit_pos))
            pValue[0] = 1;
        else
            pValue[0] = 0;
        return RT_ERR_OK;
    }

    /* Process multiple bits request - can process more than 32-bits in one field  */
    for (unprocess_len = pField->len; unprocess_len > 0; unprocess_len -= 32, i++)
    {
        if (data_bit_pos)
        {
            pValue[i] = (pData[data_word_pos] >> data_bit_pos) & ((1 << (32 - data_bit_pos)) - 1);
            data_word_pos--;
            pValue[i] |= (pData[data_word_pos] << (32 - data_bit_pos));
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
} /* end of _reg_array_field_get */


/* Function Name:
 *      _reg_array_field_set
 * Description:
 *      Set the value to one specified field of register or register array in buffer.
 * Input:
 *      reg     - register index
 *      index_1 - dimension 1 index
 *      index_2 - dimension 2 index
 *      field   - field index of the register
 *      pValue  - pointer buffer of value from the specified field of register
 * Output:
 *      pData  - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
int32
_reg_array_field_set(
    uint32  reg,
    int32   index_1,
    int32   index_2,
    uint32  field,
    uint32  *pValue,
    uint32  *pData)
{

    uint32  masks;
    uint32  field_bit_pos = 0, reg_word_num;
    uint32  data_word_pos, data_bit_pos;
    int32  i, unprocess_len;
    rtk_regField_t  *pField = NULL;


    RT_DBG(LOG_DEBUG, (MOD_HAL), "reg=%d, index_1=%d, index_2=%d, field=%d, value=0x%x", reg, index_1, index_2, field, *pValue);
    RT_PARAM_CHK(((reg >= HAL_GET_MAX_REG_IDX()) || (field >= HAL_GET_MAX_REGFIELD_IDX())), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    pField = _reg_field_find(reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    if ((HAL_GET_REG_ARRAY_OFFSET(reg) % 32) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-HAL_GET_REG_LARRAY(reg))%(32/HAL_GET_REG_ARRAY_OFFSET(reg))) * HAL_GET_REG_ARRAY_OFFSET(reg));
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-HAL_GET_REG_LPORT(reg))%(32/HAL_GET_REG_ARRAY_OFFSET(reg))) * HAL_GET_REG_ARRAY_OFFSET(reg));
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 5);
    data_bit_pos = field_bit_pos & 31;
    i = 0;

    /* Process multiple bits request - can process more than 32-bits in one field  */
    for (unprocess_len = pField->len; unprocess_len > 0; unprocess_len -= 32, i++)
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
            data_word_pos--;
            pData[data_word_pos] &= ~(masks >> (32 - data_bit_pos));
            pData[data_word_pos] |= (pValue[i] >> (32 - data_bit_pos)) & ((1 << data_bit_pos) - 1);
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
} /* end of _reg_array_field_set */


#endif /*for register performance issue*/

/* Function Name:
 *      reg_info_get
 * Description:
 *      Get register information of the specified register index.
 * Input:
 *      reg   - register index
 * Output:
 *      pData - pointer buffer of register information
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32 
reg_info_get(uint32 reg, rtk_reg_info_t *pData)
{
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "reg=%d", reg);
    RT_PARAM_CHK((pData == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX()), RT_ERR_OUT_OF_RANGE);

    if ((pHalCtrl = hal_ctrlInfo_get()) == NULL)
        return RT_ERR_FAILED;

    pData->offset = pHalCtrl->pChip_driver->pReg_list[reg].offset;
    pData->lport = pHalCtrl->pChip_driver->pReg_list[reg].lport;
    pData->hport = pHalCtrl->pChip_driver->pReg_list[reg].hport;
    pData->larray = pHalCtrl->pChip_driver->pReg_list[reg].larray;
    pData->harray = pHalCtrl->pChip_driver->pReg_list[reg].harray;
    pData->bit_offset = pHalCtrl->pChip_driver->pReg_list[reg].array_offset;
    
    if ((ret = hal_isPpBlock_check(pData->offset, &(pData->is_PpBlock))) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;   
} /* end of reg_info_get */

#if defined(CONFIG_SDK_KERNEL_LINUX)
EXPORT_SYMBOL(reg_write);
EXPORT_SYMBOL(reg_read);
EXPORT_SYMBOL(reg_array_write);
EXPORT_SYMBOL(reg_array_read);
EXPORT_SYMBOL(reg_field_write);
EXPORT_SYMBOL(reg_field_read);
EXPORT_SYMBOL(reg_array_field_write);
EXPORT_SYMBOL(reg_array_field_read);
#endif

