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
 * Purpose : register service APIs in the SDK.
 *
 * Feature : register service APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <ioal/mem32.h>
#include <hal/chipdef/allreg.h>
#include <hal/common/halctrl.h>
#include <hal/mac/reg.h>
#if defined(__MODEL_USER__) || defined(CONFIG_SDK_MODEL_MODE)
 #if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
#include <virtualmac/vmac_target.h>
 #endif
#endif

/*
 * Symbol Definition
 */
//#define CONFIG_SDK_REGFIELD_CROSS_REGISTERS
#if defined(CONFIG_SDK_REGFIELD_CROSS_REGISTERS)
#define REG_WORD_MAX    64  /* assume maximum reg_word is 10 */
#endif

/*
 * Data Declaration
 */
#if defined(__MODEL_USER__) || defined(CONFIG_SDK_MODEL_MODE)
 #if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
model_reg_write_callback_t model_reg_write_func = NULL;
model_reg_array_read_callback_t model_reg_array_read_func = NULL;
 #endif
#endif

/*
 * Macro Declaration
 */
#define REG_ADDR(pHalCtrl, reg) (pHalCtrl->pChip_driver->pReg_list[reg]->offset)


/* register semaphore handling */
#define REG_SEM_LOCK(unit)\
do {\
    if (osal_sem_mutex_take(hal_ctrl[unit].reg_sem, OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "register semaphore lock failed");\
        return RT_ERR_FAILED;\
    }\
} while(0)

#define REG_SEM_UNLOCK(unit)\
do {\
    if (osal_sem_mutex_give(hal_ctrl[unit].reg_sem) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "register semaphore unlock failed");\
        return RT_ERR_FAILED;\
    }\
} while(0)




/*
 * Function Declaration
 */
static rtk_regField_t *_reg_field_find(uint32 unit, uint32 reg, uint32 field);
static int32 _reg_addr_find(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 *pAddr);
#if defined(CONFIG_SDK_REGFIELD_CROSS_REGISTERS)
static int32
_reg_array_field_get(
    uint32  unit,
    uint32  reg,
    int32   index_1,
    int32   index_2,
    uint32  field,
    uint32  *pValue,
    uint32  *pData);
static int32
_reg_array_field_set(
    uint32  unit,
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
 *      unit  - unit id
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
_reg_field_find(uint32 unit, uint32 reg, uint32 field)
{
    uint32          i;
    hal_control_t   *pHalCtrl = NULL;
    rtk_regField_t  *pField = NULL;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return NULL;

    pField = pHalCtrl->pChip_driver->pReg_list[reg]->pFields;
    for (i = 0; i < pHalCtrl->pChip_driver->pReg_list[reg]->field_num; i++, pField++)
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
 *      unit   - unit id
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
reg_read(uint32 unit, uint32 reg, uint32 *pValue)
{
    int32   ret = RT_ERR_FAILED;
    uint32  addr = 0, num = 0, i;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, reg=%d", unit, reg);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    num = HAL_GET_REG_WORD_NUM(unit, reg);
    for (i = 0; i < num; i++)
    {
        ret = ioal_mem32_read(unit, addr+4*i, pValue+i);
        if(ret == RT_ERR_OK)
            RT_LOG(LOG_TRACE, MOD_HAL, "read addr=0x%x, value=0x%x", addr+4*i, *(pValue+i));
    }

    return ret;
} /* end of reg_read */

#if defined(__MODEL_USER__) || defined(CONFIG_SDK_MODEL_MODE)
 #if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
int32
reg_model_write_register(model_reg_write_callback_t model_reg_write_callback)
{
    model_reg_write_func = model_reg_write_callback;
    return RT_ERR_OK;
}

int32
reg_model_array_read_register(model_reg_array_read_callback_t model_reg_array_read_callback)
{
    model_reg_array_read_func = model_reg_array_read_callback;
    return RT_ERR_OK;
}
 #endif
#endif

/* Function Name:
 *      reg_write
 * Description:
 *      Write one specified register to the chip or lower layer I/O.
 * Input:
 *      unit  - unit id
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
reg_write(uint32 unit, uint32 reg, uint32 *pValue)
{
    int32   ret = RT_ERR_FAILED;
    uint32  addr = 0, num = 0, i;

#if defined(__MODEL_USER__) || defined(CONFIG_SDK_MODEL_MODE)
 #if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
    enum REG_ACCESS_TYPE type;

    vmac_getRegAccessType(&type);

    if (type == REG_TYPE_MODEL)
    {
        if(model_reg_write_func != NULL)
            model_reg_write_func(unit, reg, pValue);
        return RT_ERR_OK;
    }
  #endif
#endif

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, reg=%d, value=0x%x", unit, reg, *pValue);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    num = HAL_GET_REG_WORD_NUM(unit, reg);
    for (i = 0; i < num; i++)
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "write addr=0x%x, value=0x%x", addr+4*i, *(pValue+i));
        REG_SEM_LOCK(unit);
        ret = ioal_mem32_write(unit, addr+4*i, *(pValue+i));
        REG_SEM_UNLOCK(unit);
    }

    return ret;
} /* end of reg_write */


/* Function Name:
 *      reg_field_get
 * Description:
 *      Get the value from one specified field of register in buffer.
 * Input:
 *      unit   - unit id
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
    uint32  unit,
    uint32  reg,
    uint32  field,
    uint32  *pValue,
    uint32  *pData)
{
    uint32  field_bit_pos, reg_word_num;
    uint32  data_word_pos, data_bit_pos;
    uint32  masks, hWMask, lWMask;
    int32  i, unprocess_len;
    rtk_regField_t  *pField = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, reg=%d, field=%d, data=0x%x", unit, reg, field, *pData);
    RT_PARAM_CHK(((reg >= HAL_GET_MAX_REG_IDX(unit)) || (field >= HAL_GET_MAX_REGFIELD_IDX(unit))), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    pField = _reg_field_find(unit, reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    field_bit_pos = pField->lsp;

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(unit, reg);
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
        if (unprocess_len >= 32)
        {
            masks = -1;
        }
        else
        {
            masks = (1 << unprocess_len) - 1;
        }

        if (data_bit_pos == 0)
        {
            /* The field is perfectly align at bit 0 of uint32 */
            pValue[i] = pData[data_word_pos] & masks;
            data_word_pos--;
        }
        else
        {
            /* Consider the value may cross 2 uint32 words */
            lWMask = masks << data_bit_pos;             /* bit mask for low-word (current) data */
            hWMask = masks >> (32 - data_bit_pos);      /* bit mask for high-word data */

            pValue[i] = (pData[data_word_pos] & lWMask) >> data_bit_pos;
            data_word_pos--;
            if (hWMask != 0)
        {
                pValue[i] |= (pData[data_word_pos] & hWMask) << (32 - data_bit_pos);
        }
    }
    }/* end for */


    return RT_ERR_OK;
} /* end of reg_field_get */


/* Function Name:
 *      reg_field_set
 * Description:
 *      Set the value to one specified field of register in buffer.
 * Input:
 *      unit   - unit id
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
    uint32  unit,
    uint32  reg,
    uint32  field,
    uint32  *pValue,
    uint32  *pData)
{
    uint32  masks;
    uint32  field_bit_pos, reg_word_num;
    uint32  data_word_pos, data_bit_pos;
    uint32  hWMask, lWMask;
    int32  i, unprocess_len;
    rtk_regField_t  *pField = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, reg=%d, field=%d, value=0x%x", unit, reg, field, *pValue);
    RT_PARAM_CHK(((reg >= HAL_GET_MAX_REG_IDX(unit)) || (field >= HAL_GET_MAX_REGFIELD_IDX(unit))), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pField = _reg_field_find(unit, reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    field_bit_pos = pField->lsp;

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(unit, reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 5);
    data_bit_pos = field_bit_pos & 31;
    i = 0;

    /* Process multiple bits request - can process more than 32-bits in one field  */
    for (unprocess_len = pField->len; unprocess_len > 0; unprocess_len -= 32, i++)
    {
            if (unprocess_len >= 32)
            {
                masks = -1;
            }
            else
            {
                masks = (1 << unprocess_len) - 1;
            }

        if (data_bit_pos == 0)
        {
            /* The field is perfectly align at bit 0 of uint32 */
            pData[data_word_pos] &= ~masks;
            pData[data_word_pos] |= pValue[i] & masks;
            data_word_pos--;
        }
        else
        {
            /* Consider the value may cross 2 uint32 words */
            lWMask = masks << data_bit_pos;             /* bit mask for low-word (current) data */
            hWMask = masks >> (32 - data_bit_pos);      /* bit mask for high-word data */

            pData[data_word_pos] &= ~lWMask;
            pData[data_word_pos] |= ((pValue[i] << data_bit_pos) & lWMask);
            data_word_pos--;
            /* Consider the value may cross 2 uint32 words, check if there is any bits need to be set to high-word */
            if (hWMask != 0)
            {
                pData[data_word_pos] &= ~hWMask;
                pData[data_word_pos] |= (pValue[i] >> (32 - data_bit_pos)) & hWMask;
        }
    }
    }/* end for */

    return RT_ERR_OK;
} /* end of reg_field_set */


/* Function Name:
 *      reg_field_read
 * Description:
 *      Read the value from one specified field of register in chip.
 * Input:
 *      unit   - unit id
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
    uint32  unit,
    uint32  reg,
    uint32  field,
    uint32  *pValue)
{
#if defined(CONFIG_SDK_REGFIELD_CROSS_REGISTERS)
    uint32  data[REG_WORD_MAX];
    uint32  addr = 0, num = 0, i;
    int32   ret = RT_ERR_FAILED;

    /*RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, reg=%d, field=%d", unit, reg, field);*/
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    num = HAL_GET_REG_WORD_NUM(unit, reg);
    if (num > REG_WORD_MAX)
        return RT_ERR_FAILED;

    for (i = 0; i < num; i++)
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "read addr=0x%x", addr+4*i);
        if (ioal_mem32_read(unit, addr+4*i, (uint32 *)(data+i)) != RT_ERR_OK)
            return RT_ERR_FAILED;
    }

    if (reg_field_get(unit, reg, field, pValue, (uint32 *)(data)) != RT_ERR_OK)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
#else
    uint32  reg_word_num, data_word_pos;
    uint32  data, mask;
    uint32  addr = 0, mod32_lsp;
    int32   ret = RT_ERR_FAILED;
    rtk_regField_t  *pField = NULL;

    /*RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, reg=%d, field=%d", unit, reg, field);*/
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(unit, reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    reg_word_num = HAL_GET_REG_WORD_NUM(unit, reg);
    data_word_pos = reg_word_num - 1 - (pField->lsp >> 5);
    addr += data_word_pos*4;
    mod32_lsp = pField->lsp & 31;

    RT_LOG(LOG_TRACE, MOD_HAL, "read addr=0x%x", addr);

    if (ioal_mem32_read(unit, addr, &data) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if (pField->len < 32)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFFFFFF;

    (*pValue) = (data >> mod32_lsp) & mask;
    return RT_ERR_OK;
#endif
} /* end of reg_field_read */


/* Function Name:
 *      reg_field_write
 * Description:
 *      Write the value to one specified field of register in chip.
 * Input:
 *      unit  - unit id
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
    uint32  unit,
    uint32  reg,
    uint32  field,
    uint32  *pValue)
{
#if defined(CONFIG_SDK_REGFIELD_CROSS_REGISTERS)
    uint32  data[REG_WORD_MAX], r_data[REG_WORD_MAX];
    uint32  addr = 0, num = 0, i;
    int32   ret = RT_ERR_FAILED;

    /*RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, reg=%d, field=%d, value=0x%x", unit, reg, field, *pValue);*/
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    REG_SEM_LOCK(unit);

    num = HAL_GET_REG_WORD_NUM(unit, reg);
    if (num > REG_WORD_MAX)
    {
        REG_SEM_UNLOCK(unit);
        return RT_ERR_FAILED;
    }

    for (i = 0; i < num; i++)
    {
        if (ioal_mem32_read(unit, addr+4*i, (uint32 *)(r_data+i)) != RT_ERR_OK)
        {
            REG_SEM_UNLOCK(unit);
            return RT_ERR_FAILED;
        }
    }

    //data = r_data;
    osal_memcpy(data, r_data, sizeof(data));
    if (reg_field_set(unit, reg, field, pValue, (uint32 *)(data)) != RT_ERR_OK)
    {
        REG_SEM_UNLOCK(unit);
        return RT_ERR_FAILED;
    }

    //if (data != r_data)
    if (osal_memcmp(data, r_data, sizeof(data)) != 0)
    {
        for (i = 0; i < num; i++)
        {
            RT_LOG(LOG_TRACE, MOD_HAL, "write addr=0x%x, value=0x%x", addr+4*i, data[i]);
            if (ioal_mem32_write(unit, addr+4*i, data[i]) != RT_ERR_OK)
            {
                REG_SEM_UNLOCK(unit);
                return RT_ERR_FAILED;
            }
        }
    }

    REG_SEM_UNLOCK(unit);
    return RT_ERR_OK;
#else
    uint32  reg_word_num, data_word_pos;
    uint32  data, mask;
    uint32  addr = 0, mod32_lsp;
    int32   ret = RT_ERR_FAILED;
    rtk_regField_t  *pField = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, reg=%d, field=%d, value=0x%x", unit, reg, field, *pValue);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(unit, reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    REG_SEM_LOCK(unit);
    reg_word_num = HAL_GET_REG_WORD_NUM(unit, reg);
    data_word_pos = reg_word_num - 1 - (pField->lsp >> 5);
    addr += data_word_pos*4;
    mod32_lsp = pField->lsp & 31;

    if (ioal_mem32_read(unit, addr, &data) != RT_ERR_OK)
    {
        REG_SEM_UNLOCK(unit);
        return RT_ERR_FAILED;
    }

    if (pField->len < 32)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFFFFFF;

    data &= ~(mask << mod32_lsp);
    data |= (((*pValue) & mask) << mod32_lsp);

    RT_LOG(LOG_TRACE, MOD_HAL, "write addr=0x%x, value=0x%x", addr, data);

    if (ioal_mem32_write(unit, addr, data) != RT_ERR_OK)
    {
        REG_SEM_UNLOCK(unit);
        return RT_ERR_FAILED;
    }

    REG_SEM_UNLOCK(unit);
    return RT_ERR_OK;
#endif
} /* end of reg_field_write */


/* Function Name:
 *      reg_idx2Addr_get
 * Description:
 *      Register index to physical address transfer in chip.
 * Input:
 *      unit  - unit id
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
reg_idx2Addr_get(uint32 unit, uint32 reg, uint32 *pAddr)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, reg=%d", unit, reg);
    RT_PARAM_CHK((pAddr == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);

    if ((ret = _reg_addr_find(unit, reg, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, pAddr)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of reg_idx2Addr_get */


/* Function Name:
 *      reg_idxMax_get
 * Description:
 *      Get the maximum value of register index in chip.
 * Input:
 *      unit - unit id
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
reg_idxMax_get(uint32 unit, uint32 *pMax)
{
    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d", unit);

    RT_PARAM_CHK((pMax == NULL), RT_ERR_NULL_POINTER);

    (*pMax) = HAL_GET_MAX_REG_IDX(unit);

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
 *      unit    - unit id
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
_reg_addr_find(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 *pAddr)
{
    hal_control_t *pHalCtrl = NULL;
    uint32 dim1_size = 0;
    int32  ret = RT_ERR_FAILED;
    uint32 isPpReg = 0, ppRegBlkIdx;
    uint32 portmask_index=0;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    *pAddr = REG_ADDR(pHalCtrl, reg);

    /* Check index_1 & index_2 valid or not? */
    if (index_1 != REG_ARRAY_INDEX_NONE)
    {
        if ((index_1 < pHalCtrl->pChip_driver->pReg_list[reg]->lport) || (index_1 > pHalCtrl->pChip_driver->pReg_list[reg]->hport))
            return RT_ERR_REG_ARRAY_INDEX_1;
        else if (HAL_IS_PORTMASK_INDEX(unit, reg))
        {   /* Check the index_1 in the portmask or not? */
            portmask_index = HAL_GET_PORTMASK_INDEX(unit, reg);
            if (RTK_PORTMASK_IS_PORT_CLEAR(pHalCtrl->pChip_driver->pPortmask_list[portmask_index], index_1))
            {   /* index_1 is not in the portmask, return error code */
                return RT_ERR_REG_ARRAY_INDEX_1;
            }
        }
    }

    if ((index_2 != REG_ARRAY_INDEX_NONE) && ((index_2 < pHalCtrl->pChip_driver->pReg_list[reg]->larray) || (index_2 > pHalCtrl->pChip_driver->pReg_list[reg]->harray)))
        return RT_ERR_REG_ARRAY_INDEX_2;

    if (hal_isPpBlock_check(unit, (*pAddr), &isPpReg, &ppRegBlkIdx) != RT_ERR_OK)
        return ret;

    if (isPpReg)
    {
        /* PER_PORT */
        if (index_1 != REG_ARRAY_INDEX_NONE)
            *pAddr += (index_1 * HAL_GET_MACPP_BLK_INTERVAL(unit, ppRegBlkIdx));
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            if (pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset % 32)
                *pAddr += ((index_2-pHalCtrl->pChip_driver->pReg_list[reg]->larray)/(32/pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset))*4;
            else
                *pAddr += (index_2 - pHalCtrl->pChip_driver->pReg_list[reg]->larray) * (pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset/8);
        }
    }
    else
    {
        /* Global */
        if ((index_1 != REG_ARRAY_INDEX_NONE) && (index_2 != REG_ARRAY_INDEX_NONE))
        {
            /* calculate index_1 dimension size */
            if (pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset % 32)
                dim1_size = (((pHalCtrl->pChip_driver->pReg_list[reg]->harray - pHalCtrl->pChip_driver->pReg_list[reg]->larray)/(32/pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset))+1)*4;
            else
                dim1_size = ((pHalCtrl->pChip_driver->pReg_list[reg]->harray-pHalCtrl->pChip_driver->pReg_list[reg]->larray)+1)*(pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset/8);

            /* Add index 1 offset */
            *pAddr += (index_1 - pHalCtrl->pChip_driver->pReg_list[reg]->lport) * dim1_size;

            /* Add index 2 offset */
            if (pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset % 32)
                *pAddr += ((index_2-pHalCtrl->pChip_driver->pReg_list[reg]->larray)/(32/pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset))*4;
            else
                *pAddr += (index_2 - pHalCtrl->pChip_driver->pReg_list[reg]->larray) * (pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset/8);
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            if (pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset % 32)
                *pAddr += ((index_1 - pHalCtrl->pChip_driver->pReg_list[reg]->lport)/(32/pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset))*4;
            else
                *pAddr += (index_1 - pHalCtrl->pChip_driver->pReg_list[reg]->lport) * (pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset/8);
        }
        else if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            if (pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset % 32)
                *pAddr += ((index_2 - pHalCtrl->pChip_driver->pReg_list[reg]->larray)/(32/pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset))*4;
            else
                *pAddr += (index_2 - pHalCtrl->pChip_driver->pReg_list[reg]->larray) * (pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset/8);
        }
    }

    return RT_ERR_OK;
} /* end of _reg_addr_find */

/* Function Name:
 *      reg_array_read
 * Description:
 *      Read one specified register or register array from the chip or lower layer I/O.
 * Input:
 *      unit    - unit id
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
reg_array_read(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 *pValue)
{
    uint32  addr = 0, num = 0, i;
    int32   ret = RT_ERR_FAILED;

#if defined(__MODEL_USER__) || defined(CONFIG_SDK_MODEL_MODE)
 #if !defined(CONFIG_VIRTUAL_ARRAY_ONLY)
    enum REG_ACCESS_TYPE type;

    vmac_getRegAccessType(&type);
    if (type == REG_TYPE_MODEL)
    {
        if (model_reg_array_read_func != NULL)
            model_reg_array_read_func(unit, reg, index_1, index_2, pValue);
        return RT_ERR_OK;
    }
 #endif
#endif

    /*RT_LOG(LOG_TRACE, (MOD_HAL), "unit=%d, reg=%d", unit, reg);*/

    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    if ((ret = _reg_addr_find(unit, reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;
    num = HAL_GET_REG_WORD_NUM(unit, reg);
    for (i = 0; i < num; i++)
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "read addr=0x%x", addr+4*i);
        if (ioal_mem32_read(unit, addr+4*i, (uint32 *)(pValue+i)) != RT_ERR_OK)
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
 *      unit    - unit id
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
reg_array_write(uint32 unit, uint32 reg, int32 index_1, int32 index_2, uint32 *pValue)
{
    uint32  addr = 0, num, i;
    int32   ret = RT_ERR_FAILED;

    /*RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, reg=%d", unit, reg);*/
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;


    REG_SEM_LOCK(unit);
    num = HAL_GET_REG_WORD_NUM(unit, reg);
    for (i = 0; i < num; i++)
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "write addr=0x%x, value=0x%x", addr+4*i, *(pValue+i));
        if (ioal_mem32_write(unit, addr+4*i, *(pValue+i)) != RT_ERR_OK)
        {
            REG_SEM_UNLOCK(unit);
            return RT_ERR_FAILED;
        }
    }
    REG_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of reg_array_write */

/* Function Name:
 *      reg_array_field_read
 * Description:
 *      Read the value from one specified field of register or register array in chip.
 * Input:
 *      unit    - unit id
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
    uint32  unit,
    uint32  reg,
    int32   index_1,
    int32   index_2,
    uint32  field,
    uint32  *pValue)
{
#if defined(CONFIG_SDK_REGFIELD_CROSS_REGISTERS)
    uint32  data[REG_WORD_MAX], addr, num, i;
    int32   ret = RT_ERR_FAILED;

    /*RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, reg=%d, field=%d", unit, reg, field);*/
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    num = HAL_GET_REG_WORD_NUM(unit, reg);
    if (num > REG_WORD_MAX)
        return RT_ERR_FAILED;

    for (i = 0; i < num; i++)
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "read addr=0x%x", addr+4*i);
        if (ioal_mem32_read(unit, addr+4*i, (uint32 *)(data+i)) != RT_ERR_OK)
            return RT_ERR_FAILED;
    }

    return _reg_array_field_get(unit, reg, index_1, index_2, field, pValue, (uint32 *)(data));
#else

    uint32  field_bit_pos = 0, reg_word_num;
    uint32  data_word_pos;
    uint32  data, mask;
    uint32  addr = 0, mod32_lsp;
    int32   ret = RT_ERR_FAILED;
    rtk_regField_t  *pField = NULL;

    /*RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, reg=%d, field=%d", unit, reg, field);*/
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(unit, reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    if ((hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset % 32) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-hal_ctrl[unit].pChip_driver->pReg_list[reg]->larray)%(32/hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset)) * hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset);
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-hal_ctrl[unit].pChip_driver->pReg_list[reg]->lport)%(32/hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset)) * hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset);
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(unit, reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 5);
    addr += data_word_pos*4;
    mod32_lsp = field_bit_pos & 31;

    RT_LOG(LOG_TRACE, MOD_HAL, "read addr=0x%x", addr);

    if (ioal_mem32_read(unit, addr, &data) != RT_ERR_OK)
        return RT_ERR_FAILED;

    if (pField->len < 32)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFFFFFF;

    (*pValue) = (data >> mod32_lsp) & mask;
    return RT_ERR_OK;
#endif
} /* end of reg_array_field_read */

/* Function Name:
 *      reg_array_field_write
 * Description:
 *      Write the value to one specified field of register or register array in chip.
 * Input:
 *      unit    - unit id
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
    uint32  unit,
    uint32  reg,
    int32   index_1,
    int32   index_2,
    uint32  field,
    uint32  *pValue)
{
#if defined(CONFIG_SDK_REGFIELD_CROSS_REGISTERS)
    uint32  data[REG_WORD_MAX], addr, num, i;
    int32   ret = RT_ERR_FAILED;

    /*RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, reg=%d, field=%d, value=0x%x", unit, reg, field, *pValue);*/
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    REG_SEM_LOCK(unit);
    if ((ret = _reg_addr_find(unit, reg, index_1, index_2, &addr)) != RT_ERR_OK)
    {
        REG_SEM_UNLOCK(unit);
        return ret;
    }

    num = HAL_GET_REG_WORD_NUM(unit, reg);
    for (i = 0; i < num; i++)
    {
        if (ioal_mem32_read(unit, addr+4*i, (uint32 *)(data+i)) != RT_ERR_OK)
        {
            REG_SEM_UNLOCK(unit);
            return RT_ERR_FAILED;
        }
    }

    if (_reg_array_field_set(unit, reg, index_1, index_2, field, pValue, (uint32 *)(data)) != RT_ERR_OK)
    {
        REG_SEM_UNLOCK(unit);
        return RT_ERR_FAILED;
    }

    for (i = 0; i < num; i++)
    {
        RT_LOG(LOG_TRACE, MOD_HAL, "write addr=0x%x, value=0x%x", addr+4*i, data[i]);
        if (ioal_mem32_write(unit, addr+4*i, data[i]) != RT_ERR_OK)
        {
            REG_SEM_UNLOCK(unit);
            return RT_ERR_FAILED;
        }
    }
    REG_SEM_UNLOCK(unit);
    return RT_ERR_OK;
#else
    uint32  field_bit_pos = 0, reg_word_num;
    uint32  data_word_pos;
    uint32  data, mask;
    uint32  addr = 0, mod32_lsp;
    int32   ret = RT_ERR_FAILED;
    rtk_regField_t  *pField = NULL;

    /*RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, reg=%d, field=%d", unit, reg, field);*/
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(unit, reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    REG_SEM_LOCK(unit);
    /* Base on pField->lsp and pField->len to process */
    if ((hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset % 32) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-hal_ctrl[unit].pChip_driver->pReg_list[reg]->larray)%(32/hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset)) * hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset);
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-hal_ctrl[unit].pChip_driver->pReg_list[reg]->lport)%(32/hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset)) * hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset);
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(unit, reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 5);
    addr += data_word_pos*4;
    mod32_lsp = field_bit_pos & 31;

    if (ioal_mem32_read(unit, addr, &data) != RT_ERR_OK)
    {
        REG_SEM_UNLOCK(unit);
        return RT_ERR_FAILED;
    }

    if (pField->len < 32)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFFFFFF;

    data &= ~(mask << mod32_lsp);
    data |= (((*pValue) & mask) << mod32_lsp);

    RT_LOG(LOG_TRACE, MOD_HAL, "write addr=0x%x, value=0x%x", addr, data);

    if (ioal_mem32_write(unit, addr, data) != RT_ERR_OK)
    {
        REG_SEM_UNLOCK(unit);
        return RT_ERR_FAILED;
    }

    REG_SEM_UNLOCK(unit);
    return RT_ERR_OK;
#endif
} /* end of reg_array_field_write */

/* Function Name:
 *      reg_array_field_write_unproct
 * Description:
 *      Write the value to one specified field of register or register array in chip without mutex protection.
 * Input:
 *      unit    - unit id
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
 *      1. Without semaphore pretection, race condition preventation should be done in upper layer.
 *      2. The function is supported 32-bit register access.
 */
int32
reg_array_field_write_unproct(
    uint32  unit,
    uint32  reg,
    int32   index_1,
    int32   index_2,
    uint32  field,
    uint32  *pValue)
{
    uint32  field_bit_pos = 0, reg_word_num;
    uint32  data_word_pos;
    uint32  data, mask;
    uint32  addr = 0, mod32_lsp;
    int32   ret = RT_ERR_FAILED;
    rtk_regField_t  *pField = NULL;

    /*RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, reg=%d, field=%d", unit, reg, field);*/
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);

    if ((ret = _reg_addr_find(unit, reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(unit, reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    if ((hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset % 32) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-hal_ctrl[unit].pChip_driver->pReg_list[reg]->larray)%(32/hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset)) * hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset);
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-hal_ctrl[unit].pChip_driver->pReg_list[reg]->lport)%(32/hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset)) * hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset);
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(unit, reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 5);
    addr += data_word_pos*4;
    mod32_lsp = field_bit_pos & 31;

    if (ioal_mem32_read(unit, addr, &data) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    if (pField->len < 32)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFFFFFF;

    data &= ~(mask << mod32_lsp);
    data |= (((*pValue) & mask) << mod32_lsp);

    RT_LOG(LOG_TRACE, MOD_HAL, "write addr=0x%x, value=0x%x", addr, data);

    if (ioal_mem32_write(unit, addr, data) != RT_ERR_OK)
    {
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of reg_array_field_write_unproct */


/* Function Name:
 *      reg_array_field_write1toClear
 * Description:
 *      Write the value to one specified field of register or register array in chip.
 * Input:
 *      unit    - unit id
 *      reg     - register index
 *      index_1 - dimension 1 index
 *      index_2 - dimension 2 index
 *      field   - field index of the register
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
reg_array_field_write1toClear(
    uint32  unit,
    uint32  reg,
    int32   index_1,
    int32   index_2,
    uint32  field)
{
    uint32  field_bit_pos = 0, reg_word_num;
    uint32  data_word_pos;
    uint32  data, mask;
    uint32  addr = 0, mod32_lsp;
    int32   ret = RT_ERR_FAILED;
    rtk_regField_t  *pField = NULL;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, reg=%d, field=%d", unit, reg, field);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);

    if ((ret = _reg_addr_find(unit, reg, index_1, index_2, &addr)) != RT_ERR_OK)
        return ret;

    pField = _reg_field_find(unit, reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    REG_SEM_LOCK(unit);
    /* Base on pField->lsp and pField->len to process */
    if ((hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset % 32) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-hal_ctrl[unit].pChip_driver->pReg_list[reg]->larray)%(32/hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset)) * hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset);
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-hal_ctrl[unit].pChip_driver->pReg_list[reg]->lport)%(32/hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset)) * hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset);
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(unit, reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 5);
    addr += data_word_pos*4;
    mod32_lsp = field_bit_pos & 31;

    if (ioal_mem32_read(unit, addr, &data) != RT_ERR_OK)
    {
        REG_SEM_UNLOCK(unit);
        return RT_ERR_FAILED;
    }

    if (pField->len < 32)
        mask = (1 << pField->len) - 1;
    else
        mask = 0xFFFFFFFF;

    data &= mask << mod32_lsp;

    if (ioal_mem32_write(unit, addr, data) != RT_ERR_OK)
    {
        REG_SEM_UNLOCK(unit);
        return RT_ERR_FAILED;
    }

    REG_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of reg_array_field_write */

#if defined(CONFIG_SDK_REGFIELD_CROSS_REGISTERS)
/* Function Name:
 *      _reg_array_field_get
 * Description:
 *      Get the value from one specified field of register or register array in buffer.
 * Input:
 *      unit    - unit id
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
    uint32  unit,
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
    uint32  masks, hWMask, lWMask;
    rtk_regField_t  *pField = NULL;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, reg=%d, index_1=%d, index_2=%d, field=%d, data=0x%x", unit, reg, index_1, index_2, field, *pData);
    RT_PARAM_CHK(((reg >= HAL_GET_MAX_REG_IDX(unit)) || (field >= HAL_GET_MAX_REGFIELD_IDX(unit))), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pField = _reg_field_find(unit, reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    if ((hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset % 32) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-hal_ctrl[unit].pChip_driver->pReg_list[reg]->larray)%(32/hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset)) * hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset);
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-hal_ctrl[unit].pChip_driver->pReg_list[reg]->lport)%(32/hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset)) * hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset);
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(unit, reg);
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
        if (unprocess_len >= 32)
        {
            masks = -1;
        }
        else
        {
            masks = (1 << unprocess_len) - 1;
        }

        if (data_bit_pos == 0)
        {
            /* The field is perfectly align at bit 0 of uint32 */
            pValue[i] = pData[data_word_pos] & masks;
            data_word_pos--;
        }
        else
        {
            /* Consider the value may cross 2 uint32 words */
            lWMask = masks << data_bit_pos;             /* bit mask for low-word (current) data */
            hWMask = masks >> (32 - data_bit_pos);      /* bit mask for high-word data */

            pValue[i] = (pData[data_word_pos] & lWMask) >> data_bit_pos;
            data_word_pos--;
            if (hWMask != 0)
        {
                pValue[i] |= (pData[data_word_pos] & hWMask) << (32 - data_bit_pos);
        }
    }
    }/* end for */

    return RT_ERR_OK;
} /* end of _reg_array_field_get */

/* Function Name:
 *      _reg_array_field_set
 * Description:
 *      Set the value to one specified field of register or register array in buffer.
 * Input:
 *      unit    - unit id
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
    uint32  unit,
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
    uint32  hWMask, lWMask;
    rtk_regField_t  *pField = NULL;

    RT_LOG(LOG_DEBUG, (MOD_HAL), "unit=%d, reg=%d, index_1=%d, index_2=%d, field=%d, value=0x%x", unit, reg, index_1, index_2, field, *pValue);
    RT_PARAM_CHK(((reg >= HAL_GET_MAX_REG_IDX(unit)) || (field >= HAL_GET_MAX_REGFIELD_IDX(unit))), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pValue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pField = _reg_field_find(unit, reg, field);
    RT_PARAM_CHK((NULL == pField), RT_ERR_NULL_POINTER);

    /* Base on pField->lsp and pField->len to process */
    if ((hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset % 32) == 0)
    {
        field_bit_pos = pField->lsp;
    }
    else
    {
        if (index_2 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_2-hal_ctrl[unit].pChip_driver->pReg_list[reg]->larray)%(32/hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset)) * hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset);
        }
        else if (index_1 != REG_ARRAY_INDEX_NONE)
        {
            field_bit_pos = (((index_1-hal_ctrl[unit].pChip_driver->pReg_list[reg]->lport)%(32/hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset)) * hal_ctrl[unit].pChip_driver->pReg_list[reg]->bit_offset);
        }
    }

    /* Caculate the data LSB bit */
    reg_word_num = HAL_GET_REG_WORD_NUM(unit, reg);
    data_word_pos = reg_word_num - 1 - (field_bit_pos >> 5);
    data_bit_pos = field_bit_pos & 31;
    i = 0;

    /* Process multiple bits request - can process more than 32-bits in one field  */
    for (unprocess_len = pField->len; unprocess_len > 0; unprocess_len -= 32, i++)
    {
            if (unprocess_len >= 32)
            {
                masks = -1;
            }
            else
            {
                masks = (1 << unprocess_len) - 1;
            }

        if (data_bit_pos == 0)
        {
            /* The field is perfectly align at bit 0 of uint32 */
            pData[data_word_pos] &= ~masks;
            pData[data_word_pos] |= pValue[i] & masks;
            data_word_pos--;
        }
        else
        {
            /* Consider the value may cross 2 uint32 words */
            lWMask = masks << data_bit_pos;             /* bit mask for low-word (current) data */
            hWMask = masks >> (32 - data_bit_pos);      /* bit mask for high-word data */

            pData[data_word_pos] &= ~lWMask;
            pData[data_word_pos] |= ((pValue[i] << data_bit_pos) & lWMask);
            data_word_pos--;
            /* Consider the value may cross 2 uint32 words, check if there is any bits need to be set to high-word */
            if (hWMask != 0)
            {
                pData[data_word_pos] &= ~hWMask;
                pData[data_word_pos] |= (pValue[i] >> (32 - data_bit_pos)) & hWMask;
        }
    }
    }/* end for */

    return RT_ERR_OK;
} /* end of _reg_array_field_set */
#endif

/* Function Name:
 *      reg_info_get
 * Description:
 *      Get register information of the specified register index.
 * Input:
 *      unit  - unit id
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
reg_info_get(uint32 unit, uint32 reg, rtk_reg_info_t *pData)
{
    uint32  ppBlkIdx = 0;
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, reg=%d", unit, reg);
    RT_PARAM_CHK((pData == NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    pData->offset = pHalCtrl->pChip_driver->pReg_list[reg]->offset;
    pData->lport = pHalCtrl->pChip_driver->pReg_list[reg]->lport;
    pData->hport = pHalCtrl->pChip_driver->pReg_list[reg]->hport;
    pData->larray = pHalCtrl->pChip_driver->pReg_list[reg]->larray;
    pData->harray = pHalCtrl->pChip_driver->pReg_list[reg]->harray;
    pData->bit_offset = pHalCtrl->pChip_driver->pReg_list[reg]->bit_offset;

    if ((ret = hal_isPpBlock_check(unit, pData->offset, &(pData->is_PpBlock), &ppBlkIdx)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of reg_info_get */

#if defined(CONFIG_SDK_DUMP_REG_WITH_NAME)
/* Function Name:
 *      reg_name_get
 * Description:
 *      Get register name of the specified register index.
 * Input:
 *      unit  - unit id
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
reg_name_get(uint32 unit, uint32 reg, char *pData)
{

    hal_control_t   *pHalCtrl = NULL;

    RT_LOG(LOG_TRACE, MOD_HAL, "unit=%d, reg=%d", unit, reg);
    RT_PARAM_CHK((reg >= HAL_GET_MAX_REG_IDX(unit)), RT_ERR_OUT_OF_RANGE);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if((pHalCtrl->pChip_driver == NULL) || (pHalCtrl->pChip_driver->pReg_list == NULL))
    {
        return RT_ERR_NULL_POINTER;
    }else{
        osal_memcpy(pData, pHalCtrl->pChip_driver->pReg_list[reg]->name, REG_NAME_LEN);
    }

    return RT_ERR_OK;
} /* end of reg_info_get */
#endif
