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

#ifndef __IOAL_MEM32_H__
#define __IOAL_MEM32_H__

/*
 * Include Files
 */

#include <common/type.h>
#include <soc/type.h>


/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

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
 */
extern int32
ioal_mem32_check(uint32 *addr);

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
extern int32 
ioal_mem32_read(uint32 addr, uint32 *pVal);

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
extern int32 
ioal_mem32_write(uint32 addr, uint32 val);

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
extern int32
ioal_mem32_field_read(uint32 addr, uint32 offset, uint32 mask, uint32 *pVal);

/* Function Name:
 *      ioal_mem32_field_write
 * Description:
 *      Write the value to the field of register.
 * Input:
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
extern int32
ioal_mem32_field_write(uint32 addr, uint32 offset, uint32 mask, uint32 val);


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
extern int32
ioal_socMem32_read(uint32 addr, uint32 *pVal);

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
extern int32 
ioal_socMem32_write(uint32 addr, uint32 val);

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
ioal_ponNic_read(uint32 addr, uint32 *pVal);

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
int32 ioal_ponNic_write(uint32 addr, uint32 val);


extern int32
ioal_mem32_debugMode_set(rtk_enable_t mode);
#endif  /* __IOAL_MEM32_H__ */

