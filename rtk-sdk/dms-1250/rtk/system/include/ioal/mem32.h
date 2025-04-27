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
#include <soc/type.h>
#ifdef __BOOTLOADER__

#else
#include <common/rt_autoconf.h>
#endif
#include <common/type.h>

/*
 * Symbol Definition
 */

/*
 * Date Type Declaration
 */
typedef enum ioal_cb_readWRite_e
{
    IOAL_CB_READ=0,
    IOAL_CB_WRITE,
}ioal_cb_readWRite_t;

typedef int32 (*ioal_mem32_read_t)(uint32 unit, uintptr addr, uint32 *pVal);
typedef int32 (*ioal_mem32_write_t)(uint32 unit, uintptr addr, uint32 val);


/*
 * Function Declaration
 */

/* Function Name:
 *      ioal_mem32_hraCb_register
 * Description:
 *      Register callback function of Hardware Remote Access API
 * Input:
 *      unit        - unit id
 *      readWrite   - Read function or Write function
 *      func        - callback function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - valid
 *      RT_ERR_FAILED - invalid
 * Note:
 *      None
 */
int32
ioal_mem32_hraCb_register(uint32 unit, ioal_cb_readWRite_t readWrite, void *func);

/* Function Name:
 *      ioal_mem32_check
 * Description:
 *      Check the register address is valid or not for the specified chip.
 * Input:
 *      unit - unit id
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
extern int32
ioal_mem32_check(uint32 unit, uintptr *addr);

/* Function Name:
 *      ioal_mem32_read
 * Description:
 *      Get the value from register.
 * Input:
 *      unit - unit id
 *      addr - register address
 * Output:
 *      pVal - pointer buffer of the register value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Support single unit right now and ignore unit
 *      2. When we support the multiple chip in future, we will check the input unit
 */
extern int32
ioal_mem32_read(uint32 unit, uintptr addr, uint32 *pVal);

/* Function Name:
 *      ioal_mem32_write
 * Description:
 *      Set the value to register.
 * Input:
 *      unit - unit id
 *      addr - register address
 *      val  - the value to write register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Support single unit right now and ignore unit
 *      2. When we support the multiple chip in future, we will check the input unit
 */
extern int32
ioal_mem32_write(uint32 unit, uintptr addr, uint32 val);

/* Function Name:
 *      ioal_mem32_field_read
 * Description:
 *      Read the value from the field of register.
 * Input:
 *      unit   - unit id
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
ioal_mem32_field_read(uint32 unit, uintptr addr, uint32 offset, uint32 mask, uint32 *pVal);

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
extern int32
ioal_mem32_field_write(uint32 unit, uintptr addr, uint32 offset, uint32 mask, uint32 val);

/* Function Name:
 *      ioal_soc_mem32_read
 * Description:
 *      Get the value from register of SoC
 * Input:
 *      unit - unit id
 *      addr - register address
 * Output:
 *      pVal - pointer buffer of the register value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32
ioal_soc_mem32_read(uint32 unit, uintptr addr, uint32 *pVal);

/* Function Name:
 *      ioal_soc_mem32_write
 * Description:
 *      Set the value to SoC related register.
 * Input:
 *      unit - unit id
 *      addr - register address
 *      val  - the value to write register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32
ioal_soc_mem32_write(uint32 unit, uintptr addr, uint32 val);

#endif  /* __IOAL_MEM32_H__ */

