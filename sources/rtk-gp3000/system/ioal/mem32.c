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
 * Purpose : I/O read/write APIs in the SDK.
 *
 * Feature : I/O read/write APIs
 *
 */

/*
 * Include Files
 */
#include <common/error.h>
#include <ioal/ioal.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#include <osal/print.h>

/*
 * Symbol Definition
 */
static rtk_enable_t reg_dbg = DISABLED;

/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
int32
ioal_mem32_debugMode_set(rtk_enable_t mode)
{
    int32 ret;
    
    reg_dbg = mode;
    ret = RT_ERR_OK;
    
#if defined(CONFIG_LINUX_USER_SHELL)
    ret = debug_mem32_debugMode_set(mode);
#endif
    return RT_ERR_OK;
}


/* Function Name:
 *      ioal_mem32_check
 * Description:
 *      Check the register address is valid or not for the specified chip.
 * Input:
 *      addr - register address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - valid
 *      RT_ERR_FAILED - invalid
 * Note:
 *  1. The addr value should be offset address from the chip base in multiple chip.
 *  2. For some single chip solution, it maybe input physical address value.
 */
int32
ioal_mem32_check(uint32 *addr)
{
    uint32 base;
    int32 ret = RT_ERR_FAILED;


    RT_ERR_CHK(ioal_init_memRegion_get(IOAL_MEM_SWCORE, &base), ret);
#if defined(CONFIG_SOC_ASICDRV_TEST)
    *addr = *addr % DRV_VIRTUAL_REG_SIZE;
    *addr += base;
#else
    *addr += base;
#endif

    return RT_ERR_OK;
} /* end of ioal_mem32_check */


/* Function Name:
 *      ioal_mem32_read
 * Description:
 *      Get the value from register.
 * Input:
 *      addr - register address
 * Output:
 *      pVal - pointer buffer of the register value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
ioal_mem32_read(uint32 addr, uint32 *pVal)
{
    int32 ret = RT_ERR_FAILED;
    uint32 ori_addr;

    ori_addr = addr;
    RT_ERR_CHK(ioal_mem32_check(&addr), ret);

    *pVal = IOAL_MEM32_READ(addr);
#if 0
    if(0x23790371==*pVal)
    {
        osal_printf("ioal_mem32_read 0x%x: 0x%x\n\r", addr, *pVal);
    }
#endif

    if(reg_dbg == ENABLED)
        osal_printf("R[0x%8.8x]=0x%8.8x\n\r", ori_addr, *pVal);
    return ret;
} /* end of ioal_mem32_read */


/* Function Name:
 *      ioal_mem32_write
 * Description:
 *      Set the value to register.
 * Input:
 *      addr - register address
 *      val  - the value to write register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32 ioal_mem32_write(uint32 addr, uint32 val)
{
    int32 ret = RT_ERR_FAILED;
    uint32 ori_addr;

    ori_addr = addr;

    RT_ERR_CHK(ioal_mem32_check(&addr), ret);

    (void)(IOAL_MEM32_WRITE(addr, val));

    if(reg_dbg == ENABLED)
        osal_printf("W[0x%8.8x]=0x%8.8x\n\r", ori_addr, val);

    return ret;
} /* end of ioal_mem32_write */



/* Function Name:
 *      ioal_socMem32_read
 * Description:
 *      Get the value from soc memory.
 * Input:
 *      addr - soc memory address
 * Output:
 *      pVal - pointer buffer of the read value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
ioal_socMem32_read(uint32 addr, uint32 *pVal)
{
    int32 ret = RT_ERR_OK;

#if defined(CONFIG_LINUX_USER_SHELL)
    *pVal = io_mii_socMemory_read(addr);
#else
    *pVal = IOAL_MEM32_READ(addr);
#endif

    if(0x23790371==*pVal)
    {
        osal_printf("ioal_socMem32_read 0x%x: 0x%x\n\r", addr, *pVal);
    }


    if(reg_dbg == ENABLED)
        osal_printf("R[0x%8.8x]=0x%8.8x\n\r", addr, *pVal);
    return ret;
} /* end of ioal_socMem32_read */


/* Function Name:
 *      ioal_socMem32_write
 * Description:
 *      Set the value to soc memory.
 * Input:
 *      addr - soc memory address address
 *      val  - the value to write to the given soc memory address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32 ioal_socMem32_write(uint32 addr, uint32 val)
{
    int32 ret = RT_ERR_OK;

#if defined(CONFIG_LINUX_USER_SHELL)
    (void)io_mii_socMemory_write(addr, val);
#else
    (void)(IOAL_MEM32_WRITE(addr, val));
#endif

    if(reg_dbg == ENABLED)
        osal_printf("W[0x%8.8x]=0x%8.8x\n\r", addr, val);

    return ret;
} /* end of ioal_socMem32_write */

/* Function Name:
 *      ioal_ponNic_read
 * Description:
 *      Get the value from pon nic memory.
 * Input:
 *      addr - pon nic memory address
 * Output:
 *      pVal - pointer buffer of the read value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
ioal_ponNic_read(uint32 addr, uint32 *pVal)
{
    int32 ret = RT_ERR_OK;

    *pVal = IOAL_MEM32_READ(addr);

    //osal_printf("ioal_ponNic_read 0x%x: 0x%x\n\r", addr, *pVal);

    if(reg_dbg == ENABLED)
        osal_printf("R[0x%8.8x]=0x%8.8x\n\r", addr, *pVal);
    return ret;
} /* end of ioal_ponNic_read */

/* Function Name:
 *      ioal_ponNic_write
 * Description:
 *      Set the value to pon nic memory.
 * Input:
 *      addr - pon nic memory address address
 *      val  - the value to write to the given pon nic memory address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32 ioal_ponNic_write(uint32 addr, uint32 val)
{
    int32 ret = RT_ERR_OK;

    (void)(IOAL_MEM32_WRITE(addr, val));

    if(reg_dbg == ENABLED)
        osal_printf("W[0x%8.8x]=0x%8.8x\n\r", addr, val);

    return ret;
} /* end of ioal_ponNic_write */


/* Function Name:
 *      ioal_mem32_field_read
 * Description:
 *      Read the value from the field of register.
 * Input:
 *      addr   - register address
 *      offset - field offset
 *      mask   - field mask
 * Output:
 *      pVal - pointer buffer of the register field value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
ioal_mem32_field_read(uint32 addr, uint32 offset, uint32 mask, uint32 *pVal)
{
    int32 ret = RT_ERR_FAILED;
    uint32 ori_addr;

    ori_addr = addr;

    RT_ERR_CHK(ioal_mem32_check(&addr), ret);

    *pVal = (IOAL_MEM32_READ(addr) & mask) >> offset;

#if 0
    osal_printf("ioal_mem32_field_read 0x%x [0x%x]: 0x%x\n\r", addr, offset, *pVal);
#endif

    if(reg_dbg == ENABLED)
        osal_printf("R[0x%8.8x]=0x%8.8x\n\r", ori_addr, *pVal);

    return ret;
} /* end of ioal_mem32_field_read */

/* Function Name:
 *      ioal_mem32_field_write
 * Description:
 *      Write the value to the field of register.
 * Input:
 *      unit   - unit id
 *      addr   - register address
 *      offset - field offset
 *      mask   - field mask
 *      val    - the value to write register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
ioal_mem32_field_write(uint32 addr, uint32 offset, uint32 mask, uint32 val)
{
    int32 ret = RT_ERR_FAILED;

    uint32 ori_addr;

    ori_addr = addr;

    RT_ERR_CHK(ioal_mem32_check(&addr), ret);

    (void)(IOAL_MEM32_WRITE(addr, (IOAL_MEM32_READ(addr) & ~mask) | (val << offset)));

#if 0
    osal_printf("ioal_mem32_field_write 0x%x [0x%x][0x%x]: 0x%x\n\r", addr, offset, mask, val);
#endif

    if(reg_dbg == ENABLED)
        osal_printf("W[0x%8.8x]=0x%8.8x\n\r", ori_addr, val);

    return ret;
} /* end of ioal_mem32_field_write */
